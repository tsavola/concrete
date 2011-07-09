/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_LONG_DATA_HPP
#define CONCRETE_OBJECTS_LONG_DATA_HPP

#include "long.hpp"

#include <concrete/objects/object-data.hpp>
#include <concrete/portable.hpp>

namespace concrete {

struct LongObject::Data: Object::Data {
	Portable<Value> value;

	Data(const TypeObject &type, Value value);

} CONCRETE_PACKED;

} // namespace

#endif
