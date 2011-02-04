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
	PortableObject name;
	ObjectProtocol protocol;

	TypeBlock(BlockId type_id, NoRefcountInit): ObjectBlock(type_id, NoRefcountInit())
	{
	}

	TypeBlock(const TypeObject &type): ObjectBlock(type)
	{
	}

	TypeBlock(const TypeObject &type, const StringObject &name): ObjectBlock(type), name(name)
	{
	}
} CONCRETE_PACKED;

template <typename Ops>
TypeObject type_object<Ops>::Type()
{
	return Context::Builtins().type_type;
}

template <typename Ops>
type_object<Ops> type_object<Ops>::NewBuiltin() throw (AllocError)
{
	auto id = Context::Alloc(sizeof (TypeBlock));
	concrete_trace(("type: id=%d") % id);
	auto block = static_cast<TypeBlock *> (Context::Pointer(id));
	block->refcount = 0;
	new (block) TypeBlock(id, ObjectBlock::NoRefcountInit());
	return id;
}

template <typename Ops>
type_object<Ops> type_object<Ops>::NewBuiltin(const TypeObject &type) throw (AllocError)
{
	auto id = Context::Alloc(sizeof (TypeBlock));
	concrete_trace(("type: id=%d") % id);
	new (Context::Pointer(id)) TypeBlock(type);
	return id;
}

template <typename Ops>
type_object<Ops> type_object<Ops>::New(const StringObject &name) throw (AllocError)
{
	auto id = Context::Alloc(sizeof (TypeBlock));
	concrete_trace(("type: id=%d") % id);
	new (Context::Pointer(id)) TypeBlock(Type(), name);
	return id;
}

template <typename Ops>
void type_object<Ops>::init_builtin(const StringObject &name) const
{
	type_block()->name = name;
}

template <typename Ops>
StringObject type_object<Ops>::name() const
{
	return static_cast<PortableStringObject &> (type_block()->name);
	// TODO: return type_block()->name.cast<StringObject>();
}

template <typename Ops>
ObjectProtocol &type_object<Ops>::protocol() const
{
	return type_block()->protocol;
}

template <typename Ops>
TypeBlock *type_object<Ops>::type_block() const
{
	return static_cast<TypeBlock *> (object<Ops>::object_block());
}

} // namespace

#endif
