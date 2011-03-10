/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_OBJECT_HPP
#define CONCRETE_OBJECTS_OBJECT_HPP

#include "object-decl.hpp"

#include <cassert>

#include <concrete/block.hpp>
#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/none-fwd.hpp>
#include <concrete/objects/string-decl.hpp>
#include <concrete/objects/type-decl.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct ObjectBlock: Block {
	struct NoRefcountInit {};

	PortableObject type_object;
	Portable<int32_t> refcount;

	ObjectBlock(BlockId type_id, NoRefcountInit): type_object(type_id)
	{
	}

	ObjectBlock(const TypeObject &type): type_object(type), refcount(0)
	{
	}

	TypeObject type() const
	{
		// don't use cast<TypeObject>() because it causes an infinite recursion
		return TypeObject(type_object.id());
	}

	StringObject repr(BlockId id) const;

	static void Destroy(ObjectBlock *block);

} CONCRETE_PACKED;

template <typename Ops>
TypeObject ObjectLogic<Ops>::Type()
{
	return Context::Builtins().object_type;
}

template <typename Ops>
ObjectLogic<Ops> ObjectLogic<Ops>::New()
{
	auto id = Context::Alloc(sizeof (ObjectBlock));
	new (Context::Pointer(id)) ObjectBlock(Type());
	return ObjectLogic(id);
}

template <typename Ops>
ObjectLogic<Ops>::ObjectLogic(): m_raw_id(Ops::Store(Context::None().id()))
{
	ref();
}

template <typename Ops> template <typename T>
bool ObjectLogic<Ops>::check() const
{
	TypeCheck<T> impl;
	return impl(type());
}

template <typename Ops> template <typename T>
T ObjectLogic<Ops>::cast() const
{
	assert(check<T>());
	return T(id());
}

template <typename Ops> template <typename T>
T ObjectLogic<Ops>::require() const
{
	if (!check<T>())
		throw TypeError(type());

	return T(id());
}

template <typename Ops>
TypeObject ObjectLogic<Ops>::type() const
{
	return object_block()->type();
}

template <typename Ops>
StringObject ObjectLogic<Ops>::repr() const
{
	return object_block()->repr(id());
}

template <typename Ops>
void ObjectLogic<Ops>::ref() const
{
	auto block = object_block();
	block->refcount = block->refcount + 1;
}

template <typename Ops>
void ObjectLogic<Ops>::unref() const
{
	auto block = object_block();
	int refcount = block->refcount - 1;
	block->refcount = refcount;
	assert(refcount >= 0);
	if (refcount == 0)
		ObjectBlock::Destroy(block);
}

template <typename Ops>
ObjectBlock *ObjectLogic<Ops>::object_block() const
{
	return static_cast<ObjectBlock *> (Context::Pointer(id()));
}

void ObjectInit(const TypeObject &type);

} // namespace

#endif
