/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_OBJECT_CONTENT_HPP
#define CONCRETE_OBJECTS_OBJECT_CONTENT_HPP

#include "object.hpp"

#include <cstdint>

#include <concrete/block.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

class TypeObject;

struct Object::Content: Block {
	struct NoRefcountInit {};

	Portable<int32_t> refcount;
	Portable<Object> type;

	Content(BlockId type_id, NoRefcountInit no_refcount_init);
	explicit Content(const TypeObject &type);

} CONCRETE_PACKED;

} // namespace

#endif
