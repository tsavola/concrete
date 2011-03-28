/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_TYPE_HPP
#define CONCRETE_OBJECTS_TYPE_HPP

#include "type-decl.hpp"

#include <concrete/block.hpp>
#include <concrete/context.hpp>
#include <concrete/objects/none-decl.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/string-decl.hpp>
#include <concrete/util/portable.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

struct TypeBlock: ObjectBlock {
	PortableObject name_object;
	ObjectProtocol protocol;

	TypeBlock(const NoneObject &none, BlockId type_id, NoRefcountInit no_refcount_init):
		ObjectBlock(type_id, no_refcount_init),
		name_object(none),
		protocol(none)
	{
	}

	TypeBlock(const NoneObject &none, const TypeObject &type):
		ObjectBlock(type),
		name_object(none),
		protocol(none)
	{
	}

	TypeBlock(const TypeObject &type, const StringObject &name):
		ObjectBlock(type),
		name_object(name)
	{
	}
} CONCRETE_PACKED;

template <typename Ops>
TypeObject TypeLogic<Ops>::Type()
{
	return Context::SystemObjects()->type_type;
}

template <typename Ops>
TypeLogic<Ops> TypeLogic<Ops>::NewBuiltin(const NoneObject &none)
{
	auto ret = Context::AllocBlock(sizeof (TypeBlock));
	auto block = static_cast<TypeBlock *> (ret.ptr);
	block->refcount = 0;
	new (block) TypeBlock(none, ret.id, ObjectBlock::NoRefcountInit());
	return ret.id;
}

template <typename Ops>
TypeLogic<Ops> TypeLogic<Ops>::NewBuiltin(const NoneObject &none, const TypeObject &type)
{
	auto ret = Context::AllocBlock(sizeof (TypeBlock));
	new (ret.ptr) TypeBlock(none, type);
	return ret.id;
}

template <typename Ops>
TypeLogic<Ops> TypeLogic<Ops>::New(const StringObject &name)
{
	return Context::NewBlock<TypeBlock>(Type(), name);
}

template <typename Ops>
void TypeLogic<Ops>::init_builtin(const StringObject &name) const
{
	type_block()->name_object = name;
}

template <typename Ops>
StringObject TypeLogic<Ops>::name() const
{
	return static_cast<PortableStringObject &> (type_block()->name_object);
	// TODO: return type_block()->name.cast<StringObject>();
}

template <typename Ops>
ObjectProtocol &TypeLogic<Ops>::protocol() const
{
	return type_block()->protocol;
}

template <typename Ops>
TypeBlock *TypeLogic<Ops>::type_block() const
{
	return static_cast<TypeBlock *> (ObjectLogic<Ops>::object_block());
}

void TypeTypeInit(const TypeObject &type);

} // namespace

#endif
