/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "type-content.hpp"

#include <concrete/block.hpp>
#include <concrete/context.hpp>
#include <concrete/objects/none.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

void TypeObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("type"));
}

TypeObject::Content::Content(const NoneObject &none, BlockId type_id, NoRefcountInit no_refcount_init):
	Object::Content(type_id, no_refcount_init),
	name(none),
	protocol(none)
{
}

TypeObject::Content::Content(const NoneObject &none, const TypeObject &type):
	Object::Content(type),
	name(none),
	protocol(none)
{
}

TypeObject::Content::Content(const TypeObject &type, const StringObject &name):
	Object::Content(type),
	name(name)
{
}

TypeObject TypeObject::Type()
{
	return Context::SystemObjects()->type_type;
}

TypeObject TypeObject::NewBuiltin(const NoneObject &none)
{
	auto allocation = Context::AllocBlock(sizeof (Content));
	auto content = static_cast<Content *> (allocation.ptr);
	content->refcount = 0;
	new (content) Content(none, allocation.id, Content::NoRefcountInit());
	return allocation.id;
}

TypeObject TypeObject::NewBuiltin(const NoneObject &none, const TypeObject &type)
{
	auto allocation = Context::AllocBlock(sizeof (Content));
	new (allocation.ptr) Content(none, type);
	return allocation.id;
}

TypeObject TypeObject::New(const StringObject &name)
{
	return Context::NewBlock<Content>(Type(), name);
}

TypeObject::TypeObject(BlockId id) throw ():
	Object(id)
{
}

TypeObject::TypeObject(const TypeObject &other) throw ():
	Object(other)
{
}

TypeObject &TypeObject::operator=(const TypeObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

void TypeObject::init_builtin(const StringObject &name) const
{
	content()->name = name;
}

StringObject TypeObject::name() const
{
	return content()->name->cast<StringObject>();
}

Object::Protocol &TypeObject::protocol() const
{
	return content()->protocol;
}

TypeObject::Content *TypeObject::content() const
{
	return content_pointer<Content>();
}

} // namespace
