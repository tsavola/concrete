/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "type-data.hpp"

#include <concrete/pointer.hpp>
#include <concrete/context-data.hpp>
#include <concrete/objects/none.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/portable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

CONCRETE_OBJECT_DEFAULT_IMPL(TypeObject, type_type)

TypeObject::Data::Data(const Pointer &type) throw ():
	Object::Data(type)
{
}

TypeObject::Data::Data(const TypeObject &type) throw ():
	Object::Data(type)
{
}

TypeObject::Data::Data(const TypeObject &type, const StringObject &name) throw ():
	Object::Data(type),
	name(name)
{
}

TypeObject TypeObject::NewBuiltin()
{
	auto allocated = Arena::Active().allocate(sizeof (Data));
	new (allocated.data) Data(Pointer(allocated.address));
	return TypeObject(allocated.address);
}

TypeObject TypeObject::NewBuiltin(const TypeObject &type)
{
	return NewPointer<TypeObject>(type);
}

TypeObject TypeObject::New(const StringObject &name)
{
	return NewObject<TypeObject>(name);
}

void TypeObject::init_builtin(const StringObject &name) const
{
	data()->name = name;
}

StringObject TypeObject::name() const
{
	return data()->name->cast<StringObject>();
}

PortableObjectProtocol *TypeObject::protocol() const
{
	return &data()->protocol;
}

void TypeObjectTypeInit(const TypeObject &type, const char *name)
{
	ObjectTypeInit(type, name);
}

} // namespace
