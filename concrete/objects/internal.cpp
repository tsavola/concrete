/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "internal.hpp"

#include <map>

namespace concrete {

static std::map<InternalSerial, InternalFunction> internal_functions;

void InternalBlock::Register(InternalSerial serial, InternalFunction function)
{
	internal_functions[serial] = function;
}

Object InternalBlock::call(const TupleObject &args, const DictObject &kwargs)
{
	auto i = internal_functions.find(InternalSerial(uint16_t(serial)));
	assert(i != internal_functions.end());
	return i->second(args, kwargs);
}

} // namespace
