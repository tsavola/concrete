/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_NONE_CONTENT_HPP
#define CONCRETE_OBJECTS_NONE_CONTENT_HPP

#include "none.hpp"

#include <concrete/objects/object-content.hpp>

namespace concrete {

struct NoneObject::Content: Object::Content {
	explicit Content(BlockId none_id);

} CONCRETE_PACKED;

} // namespace

#endif
