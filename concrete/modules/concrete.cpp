/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "concrete.hpp"

#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/string.hpp>

namespace concrete {

void ConcreteModuleInit(const DictObject &modules)
{
	auto dict = DictObject::NewWithCapacity(1);

	dict.set_item(StringObject::New("test"), InternalObject::New(internal::ConcreteModule_Test));

	modules.set_item(StringObject::New("concrete"), ModuleObject::New(dict));
}

} // namespace
