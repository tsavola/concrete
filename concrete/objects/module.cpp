/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "module-data.hpp"

#include <concrete/context-data.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

ModuleObject::Data::Data(const TypeObject &type, const DictObject &dict):
	Object::Data(type),
	dict(dict)
{
}

TypeObject ModuleObject::Type()
{
	return Context::Active().data()->module_type;
}

ModuleObject ModuleObject::New(const DictObject &dict)
{
	return NewObject<ModuleObject>(dict);
}

DictObject ModuleObject::dict() const
{
	return data()->dict;
}

ModuleObject::Data *ModuleObject::data() const
{
	return data_cast<Data>();
}

void ModuleObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("module"));
}

} // namespace
