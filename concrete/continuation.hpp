/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_CONTINUATION_HPP
#define CONCRETE_CONTINUATION_HPP

#include <cassert>

#include <concrete/block.hpp>
#include <concrete/context.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>

namespace concrete {

enum ContinuationOp {
	InitContinuation,
	ResumeContinuation,
	CleanupContinuation,
};

template <typename Continuation, typename Continuable>
Object ContinuableCall(
	ContinuationOp op,
	BlockId &continuation_id,
	const Continuable &continuable,
	const TupleObject *args,
	const DictObject *kwargs)
{
	bool done = false;
	Object return_value;

	switch (op) {
	case InitContinuation:
		assert(continuation_id == NoBlockId);
		assert(args && kwargs);

		continuation_id = Context::NewBlock<Continuation>();
		done = continuable.call(continuation_id, return_value, *args, *kwargs);
		break;

	case ResumeContinuation:
		assert(continuation_id != NoBlockId);

		done = continuable.resume(continuation_id, return_value);
		break;

	case CleanupContinuation:
		assert(continuation_id != NoBlockId);

		done = true;
		break;
	}

	if (done) {
		BlockId id = continuation_id;
		continuation_id = NoBlockId;

		Context::DeleteBlock<Continuation>(id);
	}

	return return_value;
}

} // namespace

#endif
