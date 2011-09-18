/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_DICT_HPP
#define CONCRETE_OBJECTS_DICT_HPP

#include <concrete/objects/object.hpp>

namespace concrete {

class DictObject: public Object {
	CONCRETE_OBJECT_DEFAULT_DECL(DictObject, Object)

	static DictObject NewEmpty();
	static DictObject New();
	static DictObject NewWithCapacity(unsigned int capacity);

	unsigned int size() const;
	void set_item(const Object &key, const Object &value) const;
	Object get_item(const Object &key) const;
	bool get_item(const Object &key, Object &value) const;
	void copy_to(const DictObject &target) const;

private:
	unsigned int capacity() const;
};

void DictObjectTypeInit(const TypeObject &type, const char *name = "dict");

} // namespace

#endif
