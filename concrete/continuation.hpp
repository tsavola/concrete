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

#include <concrete/block.hpp>
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
	Continuable();

	void set_state(BlockId state_id) throw ();

protected:
	template <typename StateType> StateType *state_pointer() const;

private:
	BlockId m_state_id;
};

template <typename ContinuationType, typename ContinuableType>
Object ContinuableCall(ContinuationOp op,
                       BlockId &state_id,
                       ContinuableType &continuable,
                       const TupleObject *args,
                       const DictObject *kwargs);

} // namespace

#include "continuation-inline.hpp"

#endif
