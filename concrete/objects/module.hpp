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
	friend class Object;

public:
	static TypeObject Type();
	static ModuleObject New(const DictObject &dict);

	ModuleObject(const ModuleObject &other) throw ();
	ModuleObject &operator=(const ModuleObject &other) throw ();

	DictObject dict() const;

public:
	struct Content;

private:
	ModuleObject(BlockId id) throw ();

	Content *content() const;
};

void ModuleObjectTypeInit(const TypeObject &type);

} // namespace

#endif
