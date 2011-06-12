/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_OBJECT_HPP
#define CONCRETE_OBJECTS_OBJECT_HPP

#include "object-partial.hpp"

#include <concrete/block.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

class NoneObject;
class TypeObject;

struct Object::Protocol: Noncopyable {
	Portable<Object> add;
	Portable<Object> repr;
	Portable<Object> str;

	Protocol();
	Protocol(const NoneObject &none) throw ();

} CONCRETE_PACKED;

void ObjectTypeInit(const TypeObject &type);

} // namespace

#include "object-inline.hpp"

#endif
