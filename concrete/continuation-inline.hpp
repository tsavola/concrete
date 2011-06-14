/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cassert>

#include <concrete/context.hpp>

namespace concrete {

template <typename StateType>
StateType *Continuable::state_pointer() const
{
	return Context::BlockPointer<StateType>(m_state_id);
}

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
