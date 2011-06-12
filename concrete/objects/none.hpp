/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_NONE_HPP
#define CONCRETE_OBJECTS_NONE_HPP

#include <concrete/objects/object-partial.hpp>

namespace concrete {

class NoneObject: public Object {
	friend class Object;

public:
	static TypeObject Type();
	static NoneObject NewBuiltin();

	NoneObject(const NoneObject &other) throw ();
	NoneObject &operator=(const NoneObject &other) throw ();

	void init_builtin(const Portable<TypeObject> &type);

protected:
	struct Content;

private:
	NoneObject(BlockId id) throw ();

	Content *content() const;
};

void NoneObjectTypeInit(const TypeObject &type);

} // namespace

#endif
