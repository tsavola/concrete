/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "module.hpp"

#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void ModuleTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("module"));
}

} // namespace
