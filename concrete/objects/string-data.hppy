/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_STRING_DATA_HPP
#define CONCRETE_OBJECTS_STRING_DATA_HPP

#include "string.hpp"

#include <cstdint>

#include <concrete/objects/object-data.hpp>
#include <concrete/portable.hpp>

namespace concrete {

struct StringObject::Data: Object::Data {
	Portable<uint32_t> length;
	char data[0];

	Data(const TypeObject &type, const char *data);
	explicit Data(const TypeObject &type);

	void init();
	size_t size() const throw ();

} CONCRETE_PACKED;

} // namespace

#endif
