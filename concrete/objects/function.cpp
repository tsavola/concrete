/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "function.hpp"

#include <concrete/block.hpp>
#include <concrete/continuation.hpp>
#include <concrete/execute.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

void FunctionTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("function"));
}

class CallContinuation: public Block {
public:
	~CallContinuation() throw ()
	{
		if (frame_id)
			Executor::Active().destroy_frame(frame_id);
	}

	PortableBlockId frame_id;

} CONCRETE_PACKED;

struct Call {
	const CodeObject code;

	bool call(BlockId state_id,
	          Object &result,
	          const TupleObject &args,
	          const DictObject &kwargs) const
	{
		auto dict = DictObject::NewWithCapacity(args.size() + kwargs.size());

		for (unsigned int i = 0; i < args.size(); i++)
			dict.set_item(code.varnames().get_item(i), args.get_item(i));

		kwargs.copy_to(dict);

		BlockId frame_id = Executor::Active().new_frame(code, dict);
		State(state_id)->frame_id = frame_id;

		return false;
	}

	bool resume(BlockId state_id, Object &result) const
	{
		BlockId frame_id = State(state_id)->frame_id;
		result = Executor::Active().frame_result(frame_id);

		return true;
	}

	static CallContinuation *State(BlockId id)
	{
		return Context::BlockPointer<CallContinuation>(id);
	}
};

Object FunctionBlock::call(ContinuationOp op,
                           BlockId &state_id,
                           const TupleObject *args,
                           const DictObject *kwargs) const
{
	return ContinuableCall<CallContinuation>(op, state_id, Call { code }, args, kwargs);
}

} // namespace
