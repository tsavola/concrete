/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_MODULE_CONTENT_HPP
#define CONCRETE_OBJECTS_MODULE_CONTENT_HPP

#include "module.hpp"

#include <concrete/objects/object-content.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

struct ModuleObject::Content: Object::Content {
	const Portable<DictObject> dict;

	Content(const TypeObject &type, const DictObject &dict);

} CONCRETE_PACKED;

} // namespace

#endif
