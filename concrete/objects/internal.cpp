/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "internal.hpp"

#include <cassert>

#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

static InternalFunction internal_functions[internals::InternalCount];

Object InternalBlock::call(ContinuationOp op,
                           BlockId &continuation,
                           const TupleObject *args,
                           const DictObject *kwargs) const
{
	assert(internal_functions[serial]);
	return internal_functions[serial](op, continuation, args, kwargs);
}

void InternalRegister(InternalSerial serial, InternalFunction function)
{
	internal_functions[serial] = function;
}

void InternalInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("internal"));
}

} // namespace
