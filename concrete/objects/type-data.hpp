/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_TYPE_DATA_HPP
#define CONCRETE_OBJECTS_TYPE_DATA_HPP

#include "concrete/objects/type.hpp"

#include <concrete/objects/object-data.hpp>

namespace concrete {

struct TypeObject::Data: Object::Data {
	explicit Data(const Pointer &type) throw ();
	explicit Data(const TypeObject &type) throw ();
	Data(const TypeObject &type, const StringObject &name) throw ();

	Portable<Object>       name;
	PortableObjectProtocol protocol;
} CONCRETE_PACKED;

} // namespace

#endif
