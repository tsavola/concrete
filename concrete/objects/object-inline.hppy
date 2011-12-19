/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

template <typename ObjectType, typename... Args>
ObjectType Object::NewObject(Args... args)
{
	return NewPointer<ObjectType>(ObjectType::Type(), args...);
}

template <typename ObjectType, typename... Args>
ObjectType Object::NewCustomSizeObject(size_t size, Args... args)
{
	return NewCustomSizePointer<ObjectType>(size, ObjectType::Type(), args...);
}

template <typename ObjectType>
bool Object::check() const
{
	TypeCheck<ObjectType> impl;
	return impl(type());
}

template <typename ObjectType>
ObjectType Object::require() const
{
	if (!check<ObjectType>())
		throw TypeError(type());

	return RawAccess::Materialize<ObjectType>(address());
}

} // namespace
