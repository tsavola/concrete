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

class CallState: public Block {
public:
	~CallState() throw ()
	{
		if (frame_id)
			Executor::Active().destroy_frame(frame_id);
	}

	PortableBlockId frame_id;

} CONCRETE_PACKED;

class Call: public Continuable {
public:
	explicit Call(const CodeObject &code) throw ():
		m_code(code)
	{
	}

	bool call(Object &result, const TupleObject &args, const DictObject &kwargs) const
	{
		auto dict = DictObject::NewWithCapacity(args.size() + kwargs.size());

		for (unsigned int i = 0; i < args.size(); i++)
			dict.set_item(m_code.varnames().get_item(i), args.get_item(i));

		kwargs.copy_to(dict);

		BlockId frame_id = Executor::Active().new_frame(m_code, dict);
		state()->frame_id = frame_id;

		return false;
	}

	bool resume(Object &result) const
	{
		BlockId frame_id = state()->frame_id;
		result = Executor::Active().frame_result(frame_id);

		return true;
	}

private:
	CallState *state() const
	{
		return state_pointer<CallState>();
	}

	const CodeObject m_code;
};

Object FunctionBlock::call(ContinuationOp op,
                           BlockId &state_id,
                           const TupleObject *args,
                           const DictObject *kwargs) const
{
	Call call(code);
	return ContinuableCall<CallState>(op, state_id, call, args, kwargs);
}

} // namespace
