/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_CODE_CONTENT_HPP
#define CONCRETE_OBJECTS_CODE_CONTENT_HPP

#include "code.hpp"

#include <concrete/objects/object-content.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

struct CodeObject::Content: Object::Content {
	Portable<uint32_t> stacksize;
	Portable<BytesObject> code;
	Portable<TupleObject> consts;
	Portable<TupleObject> names;
	Portable<TupleObject> varnames;

	Content(const TypeObject &type,
	        unsigned int stacksize,
	        const BytesObject &code,
	        const TupleObject &consts,
	        const TupleObject &names,
	        const TupleObject &varnames);

} CONCRETE_PACKED;

} // namespace

#endif
