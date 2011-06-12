/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "none-content.hpp"

#include <concrete/context.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void NoneObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("none"));

	type.protocol().repr  = InternalObject::New(internal::ObjectType_Repr);
	type.protocol().str   = InternalObject::New(internal::ObjectType_Str);
}

NoneObject::Content::Content(BlockId none_id):
	Object::Content(none_id, NoRefcountInit())
{
}

TypeObject NoneObject::Type()
{
	return Context::SystemObjects()->none_type;
}

NoneObject NoneObject::NewBuiltin()
{
	auto allocated = Context::AllocBlock(sizeof (Content));
	new (allocated.ptr) Content(allocated.id);
	return allocated.id;
}

NoneObject::NoneObject(BlockId id) throw ():
	Object(id)
{
}

NoneObject::NoneObject(const NoneObject &other) throw ():
	Object(other)
{
}

NoneObject &NoneObject::operator=(const NoneObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

void NoneObject::init_builtin(const Portable<TypeObject> &type)
{
	content()->type = type;
}

NoneObject::Content *NoneObject::content() const
{
	return content_pointer<Content>();
}

} // namespace
