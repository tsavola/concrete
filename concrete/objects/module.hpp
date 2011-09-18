/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_MODULE_HPP
#define CONCRETE_OBJECTS_MODULE_HPP

#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>

namespace concrete {

class ModuleObject: public Object {
	CONCRETE_OBJECT_DEFAULT_DECL(ModuleObject, Object)

	static ModuleObject New(const DictObject &dict);

	DictObject dict() const;
};

void ModuleObjectTypeInit(const TypeObject &type, const char *name = "module");

} // namespace

#endif
