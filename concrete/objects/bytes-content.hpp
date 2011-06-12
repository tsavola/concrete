/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_BYTES_CONTENT_HPP
#define CONCRETE_OBJECTS_BYTES_CONTENT_HPP

#include "bytes.hpp"

#include <concrete/objects/object-content.hpp>

namespace concrete {

struct BytesObject::Content: Object::Content {
	uint8_t data[0];

	Content(const TypeObject &type, const uint8_t *data);

	size_t size() const throw ();

} CONCRETE_PACKED;

} // namespace

#endif
