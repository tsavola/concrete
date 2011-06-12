/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_TYPE_HPP
#define CONCRETE_OBJECTS_TYPE_HPP

#include <concrete/objects/object-partial.hpp>

namespace concrete {

class NoneObject;
class StringObject;

class TypeObject: public Object {
	friend class Object;
	friend struct Object::Content;

public:
	static TypeObject Type();

	static TypeObject NewBuiltin(const NoneObject &none);
	static TypeObject NewBuiltin(const NoneObject &none, const TypeObject &type);
	static TypeObject New(const StringObject &name);

	TypeObject(const TypeObject &other) throw ();
	TypeObject &operator=(const TypeObject &other) throw ();

	void init_builtin(const StringObject &name) const;

	StringObject name() const;
	Object::Protocol &protocol() const;

protected:
	struct Content;

private:
	TypeObject(BlockId id) throw ();

	Content *content() const;
};

void TypeObjectTypeInit(const TypeObject &type);

template <typename ObjectType>
struct TypeCheck {
	bool operator()(const TypeObject &type);
};

} // namespace

#include "type-inline.hpp"

#endif
