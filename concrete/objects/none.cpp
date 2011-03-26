/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "none.hpp"

#include <concrete/objects/internal.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void NoneTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("none"));

	type.protocol().repr  = InternalObject::New(internal::ObjectType_Repr);
	type.protocol().str   = InternalObject::New(internal::ObjectType_Str);
}

} // namespace
