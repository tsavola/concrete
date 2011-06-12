/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_TUPLE_CONTENT_HPP
#define CONCRETE_OBJECTS_TUPLE_CONTENT_HPP

#include "tuple.hpp"

#include <concrete/objects/object-content.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

struct TupleObject::Content: Object::Content {
	Portable<Object> items[0];

	explicit Content(const TypeObject &type);
	~Content() throw ();

	unsigned int size() const throw ();

} CONCRETE_PACKED;

} // namespace

#endif
