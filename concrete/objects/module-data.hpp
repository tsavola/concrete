/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_MODULE_DATA_HPP
#define CONCRETE_OBJECTS_MODULE_DATA_HPP

#include "module.hpp"

#include <concrete/objects/object-data.hpp>
#include <concrete/portable.hpp>

namespace concrete {

struct ModuleObject::Data: Object::Data {
	const Portable<DictObject> dict;

	Data(const TypeObject &type, const DictObject &dict);

} CONCRETE_PACKED;

} // namespace

#endif
