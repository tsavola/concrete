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
	NoneBlock(BlockId none_id): ObjectBlock(none_id, NoRefcountInit())
	{
	}
} CONCRETE_PACKED;

template <typename Ops>
TypeObject NoneLogic<Ops>::Type()
{
	return Context::Builtins().none_type;
}

template <typename Ops>
NoneLogic<Ops> NoneLogic<Ops>::NewBuiltin()
{
	auto ret = Context::Active().arena().alloc(sizeof (NoneBlock));
	new (ret.ptr) NoneBlock(ret.id);
	return ret.id;
}

template <typename Ops>
void NoneLogic<Ops>::init_builtin(const TypeObject &type)
{
	ObjectLogic<Ops>::object_block()->type_object = type;
}

void NoneInit(const TypeObject &type);

} // namespace

#endif
