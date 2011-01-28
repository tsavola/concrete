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

#include "none-decl.hpp"

#include <concrete/objects/object.hpp>
#include <concrete/objects/type-decl.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct NoneBlock: ObjectBlock {
	NoneBlock(BlockId none_id): ObjectBlock(no_refcount_init, none_id)
	{
	}

} CONCRETE_PACKED;

inline TypeObject NoneObject::Type()
{
	return Context::Builtins().none_type;
}

inline NoneObject NoneObject::NewBuiltin() throw (AllocError)
{
	auto id = Context::Alloc(sizeof (NoneBlock));
	new (Context::Pointer(id)) NoneBlock(id);
	return id;
}

inline void NoneObject::init_builtin(const TypeObject &type)
{
	object_block()->type_object = type;
}

} // namespace

#endif
