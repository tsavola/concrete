/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cassert>

#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

template <typename ObjectType>
BlockId Object::RawAccess::Extract(const ObjectType &object) throw ()
{
	return object.id();
}

template <typename ObjectType>
ObjectType Object::RawAccess::Materialize(BlockId id) throw ()
{
	return ObjectType(id);
}

template <typename ObjectType>
ObjectType Object::RawAccess::Materialize(BlockId id, ObjectType *) throw ()
{
	return Materialize<ObjectType>(id);
}

template <typename ObjectType>
bool Object::check() const
{
	TypeCheck<ObjectType> impl;
	return impl(type());
}

template <typename ObjectType>
ObjectType Object::cast() const
{
	assert(check<ObjectType>());
	return RawAccess::Materialize<ObjectType>(id());
}

template <typename ObjectType>
ObjectType Object::require() const
{
	if (!check<ObjectType>())
		throw TypeError(type());

	return RawAccess::Materialize<ObjectType>(id());
}

template <typename ContentType>
ContentType *Object::content_pointer() const
{
	return Context::BlockPointer<ContentType>(id());
}

} // namespace
