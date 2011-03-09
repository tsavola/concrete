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
#include <concrete/objects/dict.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

class CallContinuation: public Block {
} CONCRETE_PACKED;

struct Call {
	const CodeObject code;

	bool call(BlockId continuation_id,
	          Object &return_value,
	          const TupleObject &args,
	          const DictObject &kwargs) const
	{
		return false;
	}

	bool resume(BlockId continuation_id, Object &return_value) const
	{
		return true;
	}

private:
	static CallContinuation *Continuation(BlockId id)
	{
		return static_cast<CallContinuation *> (Context::Pointer(id));
	}
};

Object FunctionBlock::call(ContinuationOp op,
                           BlockId &continuation_id,
                           const TupleObject *args,
                           const DictObject *kwargs) const
{
	return continuable_call<CallContinuation>(op, continuation_id, Call { code }, args, kwargs);
}

} // namespace
