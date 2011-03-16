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

struct ObjectProtocol {
	PortableObject add;
	PortableObject repr;
	PortableObject str;

	static Object Add(const Object &self, const Object &other);
	static Object Repr(const Object &self);
	static Object Str(const Object &self);

} CONCRETE_PACKED;

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

	TypeObject type() const throw ()
	{
		// don't use cast<TypeObject>() because it causes an infinite recursion
		return TypeObject(type_object.m_id);
	}
} CONCRETE_PACKED;

template <typename Ops>
TypeObject ObjectLogic<Ops>::Type()
{
	return Context::BuiltinObjects().object_type;
}

template <typename Ops>
ObjectLogic<Ops> ObjectLogic<Ops>::New()
{
	return ObjectLogic(Context::NewBlock<ObjectBlock>(Type()));
}

template <typename Ops>
ObjectLogic<Ops>::ObjectLogic(): m_id(Context::None().m_id)
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
	return T(m_id);
}

template <typename Ops> template <typename T>
T ObjectLogic<Ops>::require() const
{
	if (!check<T>())
		throw TypeError(type());

	return T(m_id);
}

template <typename Ops>
TypeObject ObjectLogic<Ops>::type() const
{
	return object_block()->type();
}

template <typename Ops>
const ObjectProtocol &ObjectLogic<Ops>::protocol() const
{
	return type().protocol();
}

template <typename Ops>
Object ObjectLogic<Ops>::add(const Object &other) const
{
	return ObjectProtocol::Add(*this, other);
}

template <typename Ops>
StringObject ObjectLogic<Ops>::repr() const
{
	return ObjectProtocol::Repr(*this).require<StringObject>();
}

template <typename Ops>
StringObject ObjectLogic<Ops>::str() const
{
	return ObjectProtocol::Str(*this).require<StringObject>();
}

template <typename Ops>
void ObjectLogic<Ops>::ref() const throw ()
{
	auto block = nonthrowing_object_block();
	if (block)
		block->refcount = block->refcount + 1;
}

template <typename Ops>
void ObjectLogic<Ops>::unref() const throw ()
{
	auto block = nonthrowing_object_block();
	if (block) {
		int refcount = block->refcount - 1;
		block->refcount = refcount;
		assert(refcount >= 0);
		if (refcount == 0)
			ObjectDestroy(block, block->type_object, id());
	}
}

template <typename Ops>
ObjectBlock *ObjectLogic<Ops>::object_block() const
{
	return Context::BlockPointer<ObjectBlock>(id());
}

template <typename Ops>
ObjectBlock *ObjectLogic<Ops>::nonthrowing_object_block() const throw ()
{
	return Context::NonthrowingBlockPointer<ObjectBlock>(id());
}

void ObjectInit(const TypeObject &type);
void ObjectDestroy(ObjectBlock *block, Object type, BlockId id) throw ();

} // namespace

#endif
