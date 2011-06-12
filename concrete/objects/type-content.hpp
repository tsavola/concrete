/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_TYPE_CONTENT_HPP
#define CONCRETE_OBJECTS_TYPE_CONTENT_HPP

#include "type.hpp"

#include <concrete/objects/object-content.hpp>

namespace concrete {

struct TypeObject::Content: Object::Content {
	Portable<Object> name;
	Object::Protocol protocol;

	Content(const NoneObject &none, BlockId type_id, NoRefcountInit no_refcount_init);
	Content(const NoneObject &none, const TypeObject &type);
	Content(const TypeObject &type, const StringObject &name);

} CONCRETE_PACKED;

} // namespace

#endif
