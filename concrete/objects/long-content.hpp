/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_LONG_CONTENT_HPP
#define CONCRETE_OBJECTS_LONG_CONTENT_HPP

#include "long.hpp"

#include <concrete/objects/object-content.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

struct LongObject::Content: Object::Content {
	Portable<int64_t> value;

	Content(const TypeObject &type, int64_t value);

} CONCRETE_PACKED;

} // namespace

#endif
