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

template <typename Ops>
TypeObject none_object<Ops>::Type()
{
	return Context::Builtins().none_type;
}

template <typename Ops>
none_object<Ops> none_object<Ops>::NewBuiltin() throw (AllocError)
{
	auto id = Context::Alloc(sizeof (NoneBlock));
	new (Context::Pointer(id)) NoneBlock(id);
	return id;
}

template <typename Ops>
void none_object<Ops>::init_builtin(const TypeObject &type)
{
	object<Ops>::object_block()->type_object = type;
}

} // namespace

#endif
