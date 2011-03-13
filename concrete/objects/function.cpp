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

class CallContinuation: public Block {
public:
	~CallContinuation()
	{
		if (m_frame_id)
			Executor::Active().destroy_frame(m_frame_id);
	}

	void new_frame(const CodeObject &code, const DictObject &dict)
	{
		m_frame_id = Executor::Active().new_frame(code, dict);
	}

	Object destroy_frame()
	{
		BlockId frame_id = m_frame_id;
		m_frame_id = NULL;

		return Executor::Active().destroy_frame(frame_id);
	}

private:
	PortableBlockId m_frame_id;

} CONCRETE_PACKED;

struct Call {
	const CodeObject code;

	bool call(BlockId continuation_id,
	          Object &return_value,
	          const TupleObject &args,
	          const DictObject &kwargs) const
	{
		auto dict = DictObject::New(args.size() + kwargs.size());

		for (unsigned int i = 0; i < args.size(); i++)
			dict.set_item(code.varnames().get_item(i), args.get_item(i));

		kwargs.copy_to(dict);

		Continuation(continuation_id)->new_frame(code, dict);
		return false;
	}

	bool resume(BlockId continuation_id, Object &return_value) const
	{
		return_value = Continuation(continuation_id)->destroy_frame();

		auto repr = return_value.repr();
		ConcreteTrace(("Function: value=%s") % repr.data());

		return true;
	}

private:
	static CallContinuation *Continuation(BlockId id)
	{
		return Context::BlockPointer<CallContinuation>(id);
	}
};

Object FunctionBlock::call(ContinuationOp op,
                           BlockId &continuation_id,
                           const TupleObject *args,
                           const DictObject *kwargs) const
{
	return ContinuableCall<CallContinuation>(op, continuation_id, Call { code }, args, kwargs);
}

void FunctionInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("function"));
}

} // namespace
