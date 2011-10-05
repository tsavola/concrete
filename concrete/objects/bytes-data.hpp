/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_BYTES_DATA_HPP
#define CONCRETE_OBJECTS_BYTES_DATA_HPP

#include "concrete/objects/bytes.hpp"

#include <concrete/objects/object-data.hpp>

namespace concrete {

struct BytesObject::Data: Object::Data {
	uint8_t data[0];

	Data(const TypeObject &type, const uint8_t *data);

	size_t size() const throw ();

} CONCRETE_PACKED;

} // namespace

#endif
