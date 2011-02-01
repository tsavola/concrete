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
	enum NoRefcountInit { no_refcount_init };

	PortableObject type_object;
	portable<int32_t> refcount;

	ObjectBlock(const TypeObject &type): type_object(type), refcount(0)
	{
	}

	TypeObject type() const
	{
		// don't use cast<TypeObject>() because it causes an infinite recursion
		return TypeObject(type_object.id());
	}

	StringObject repr(BlockId id) const;

private:
	friend struct object<ObjectOps>;
	friend struct object<PortableObjectOps>;

	static void Destroy(ObjectBlock *block) throw ();

	friend struct NoneBlock;
	friend struct TypeBlock;

	ObjectBlock(NoRefcountInit, BlockId type_id): type_object(type_id)
	{
	}

} CONCRETE_PACKED;

template <typename Ops>
TypeObject object<Ops>::Type()
{
	return Context::Builtins().object_type;
}

template <typename Ops>
object<Ops> object<Ops>::New() throw (AllocError)
{
	auto id = Context::Alloc(sizeof (ObjectBlock));
	new (Context::Pointer(id)) ObjectBlock(Type());
	return object(id);
}

template <typename Ops>
object<Ops>::object(): m_raw_id(Ops::store(Context::None().id()))
{
	ref();
}

template <typename Ops> template <typename T>
bool object<Ops>::check() const
{
	return type() == T::Type();
}

template <typename Ops> template <typename T>
T object<Ops>::cast() const
{
	assert(check<T>());
	return T(id());
}

template <typename Ops> template <typename T>
T object<Ops>::require() const
{
	if (!check<T>())
		throw TypeError(type());

	return T(id());
}

template <typename Ops>
TypeObject object<Ops>::type() const
{
	return object_block()->type();
}

template <typename Ops>
StringObject object<Ops>::repr() const
{
	return object_block()->repr(id());
}

template <typename Ops>
void object<Ops>::ref() const
{
	auto block = object_block();
	block->refcount = block->refcount + 1;
}

template <typename Ops>
void object<Ops>::unref() const
{
	auto block = object_block();
	int refcount = block->refcount - 1;
	block->refcount = refcount;
	assert(refcount >= 0);
	if (refcount == 0)
		ObjectBlock::Destroy(block);
}

template <typename Ops>
ObjectBlock *object<Ops>::object_block() const
{
	return static_cast<ObjectBlock *> (Context::Pointer(id()));
}

} // namespace

#endif
