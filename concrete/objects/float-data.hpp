/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_FLOAT_DATA_HPP
#define CONCRETE_OBJECTS_FLOAT_DATA_HPP

#include "concrete/objects/float.hpp"

#include <cstdint>

#include <concrete/objects/object-data.hpp>

namespace concrete {

struct FloatObject::Data: Object::Data {
	Data(const TypeObject &type, Value value);
	Data(const TypeObject &type, uint64_t raw);

	Value value() const throw ();

private:
	uint64_t m_raw;
} CONCRETE_PACKED;

} // namespace

#endif
