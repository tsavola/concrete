/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "none-data.hpp"

#include <concrete/arena.hpp>
#include <concrete/context-data.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

NoneObject::Data::Data(const Pointer &type_placeholder) throw ():
	Object::Data(type_placeholder)
{
}

TypeObject NoneObject::Type()
{
	return Context::Active().data()->none_type;
}

NoneObject NoneObject::NewBuiltin()
{
	auto allocated = Arena::Active().allocate(sizeof (Data));
	new (allocated.data) Data(Pointer(allocated.address));
	return NoneObject(allocated.address);
}

void NoneObject::init_builtin(const Portable<TypeObject> &type)
{
	data()->init_none_type(type);
}

NoneObject::Data *NoneObject::data() const
{
	return data_cast<Data>();
}

void NoneObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("none"));

	type.protocol()->repr  = InternalObject::New(internal::ObjectType_Repr);
	type.protocol()->str   = InternalObject::New(internal::ObjectType_Str);
}

} // namespace
