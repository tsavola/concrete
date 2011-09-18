/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_BOOL_HPP
#define CONCRETE_OBJECTS_BOOL_HPP

#include <concrete/objects/long.hpp>

namespace concrete {

class BoolObject: public LongObject {
	CONCRETE_OBJECT_DATALESS_DECL(BoolObject, LongObject)

	static BoolObject True();
	static BoolObject False();
	static BoolObject FromBool(bool value);
	static BoolObject NewBuiltin(bool value);
};

void BoolObjectTypeInit(const TypeObject &type, const char *name = "bool");

} // namespace

#endif
