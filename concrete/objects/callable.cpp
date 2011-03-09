/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "callable.hpp"

#include <concrete/objects/function.hpp>
#include <concrete/objects/internal.hpp>

namespace concrete {

bool CallableBlock::Check(const TypeObject &type)
{
	return false;
}

Object CallableBlock::call(ContinuationOp op,
                           BlockId &continuation,
                           const TupleObject *args,
                           const DictObject *kwargs) const
{
	auto t = type();
	Object value;

	return value;
}

} // namespace
