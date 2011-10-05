/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_CODE_DATA_HPP
#define CONCRETE_OBJECTS_CODE_DATA_HPP

#include "concrete/objects/code.hpp"

#include <concrete/objects/object-data.hpp>
#include <concrete/portable.hpp>

namespace concrete {

struct CodeObject::Data: Object::Data {
	Data(const TypeObject &type,
	        unsigned int stacksize,
	        const BytesObject &code,
	        const TupleObject &consts,
	        const TupleObject &names,
	        const TupleObject &varnames);

	Portable<uint32_t>    stacksize;
	Portable<BytesObject> bytecode;
	Portable<TupleObject> consts;
	Portable<TupleObject> names;
	Portable<TupleObject> varnames;
} CONCRETE_PACKED;

} // namespace

#endif
