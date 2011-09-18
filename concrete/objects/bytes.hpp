/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_BYTES_HPP
#define CONCRETE_OBJECTS_BYTES_HPP

#include <cstddef>
#include <cstdint>

#include <concrete/objects/object.hpp>

namespace concrete {

class BytesObject: public Object {
	CONCRETE_OBJECT_DEFAULT_DECL(BytesObject, Object)

	static BytesObject New(const uint8_t *data, size_t size);

	size_t size() const;
	const uint8_t *c_data() const;
};

void BytesObjectTypeInit(const TypeObject &type, const char *name = "bytes");

} // namespace

#endif
