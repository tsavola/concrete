/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "module-content.hpp"

#include <concrete/context.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void ModuleObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("module"));
}

ModuleObject::Content::Content(const TypeObject &type, const DictObject &dict):
	Object::Content(type),
	dict(dict)
{
}

TypeObject ModuleObject::Type()
{
	return Context::SystemObjects()->module_type;
}

ModuleObject ModuleObject::New(const DictObject &dict)
{
	return Context::NewBlock<Content>(Type(), dict);
}

ModuleObject::ModuleObject(BlockId id) throw ():
	Object(id)
{
}

ModuleObject::ModuleObject(const ModuleObject &other) throw ():
	Object(other)
{
}

ModuleObject &ModuleObject::operator=(const ModuleObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

DictObject ModuleObject::dict() const
{
	return content()->dict;
}

ModuleObject::Content *ModuleObject::content() const
{
	return content_pointer<Content>();
}

} // namespace
