/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_STRING_CONTENT_HPP
#define CONCRETE_OBJECTS_STRING_CONTENT_HPP

#include "string.hpp"

#include <cstdint>

#include <concrete/objects/object-content.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

struct StringObject::Content: Object::Content {
	Portable<uint32_t> length;
	char data[0];

	Content(const TypeObject &type, const char *data);
	explicit Content(const TypeObject &type);

	void init();
	size_t size() const throw ();

} CONCRETE_PACKED;

} // namespace

#endif
