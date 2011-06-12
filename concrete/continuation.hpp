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
#include <concrete/util/noncopyable.hpp>

namespace concrete {

enum ContinuationOp {
	InitContinuation,
	ResumeContinuation,
	CleanupContinuation,
};

class Continuable: Noncopyable {
public:
	Continuable():
		m_state_id(0)
	{
	}

	void set_state(BlockId id) throw ()
	{
		m_state_id = id;
	}

protected:
	template <typename T>
	T *state_pointer() const
	{
		return Context::BlockPointer<T>(m_state_id);
	}

private:
	BlockId m_state_id;
};

template <typename ContinuationType, typename ContinuableType>
Object ContinuableCall(ContinuationOp op,
                       BlockId &state_id,
                       ContinuableType &continuable,
                       const TupleObject *args,
                       const DictObject *kwargs)
{
	bool done = false;
	Object result;

	switch (op) {
	case InitContinuation:
		assert(state_id == 0);
		assert(args && kwargs);

		state_id = Context::NewBlock<ContinuationType>();
		continuable.set_state(state_id);
		done = continuable.call(result, *args, *kwargs);
		break;

	case ResumeContinuation:
		assert(state_id);

		continuable.set_state(state_id);
		done = continuable.resume(result);
		break;

	case CleanupContinuation:
		assert(state_id);

		continuable.set_state(state_id);
		done = true;
		break;
	}

	if (done) {
		BlockId id = state_id;
		state_id = 0;

		Context::DeleteBlock<ContinuationType>(id);
	}

	return result;
}

} // namespace

#endif
