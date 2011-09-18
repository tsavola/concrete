/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_LONG_HPP
#define CONCRETE_OBJECTS_LONG_HPP

#include <cstdint>

#include <concrete/objects/object.hpp>

namespace concrete {

class LongObject: public Object {
	CONCRETE_OBJECT_DEFAULT_DECL(LongObject, Object)

	typedef int64_t Value;

	static LongObject New(Value value);

	Value value() const;
};

template <> struct TypeCheck<LongObject> {
	bool operator()(const TypeObject &type);
};

void LongObjectTypeInit(const TypeObject &type, const char *name = "long");

} // namespace

#endif
