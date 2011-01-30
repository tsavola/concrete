/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_STRING_HPP
#define CONCRETE_OBJECTS_STRING_HPP

#include "string-decl.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <concrete/context.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/type-decl.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct StringBlock: ObjectBlock {
	portable<uint32_t> length;
	char data[0];

	StringBlock(const TypeObject &type, const char *data);

	size_t size() const
	{
		return block_size() - sizeof (StringBlock) - 1;
	}

} CONCRETE_PACKED;

template <typename Ops>
TypeObject string_object<Ops>::Type()
{
	return Context::Builtins().string_type;
}

template <typename Ops>
string_object<Ops> string_object<Ops>::New(const char *data, size_t size) throw (AllocError)
{
	auto id = Context::Alloc(sizeof (StringBlock) + size + 1);
	new (Context::Pointer(id)) StringBlock(Type(), data);
	return id;
}

template <typename Ops>
size_t string_object<Ops>::size() const
{
	return string_block()->size();
}

template <typename Ops>
size_t string_object<Ops>::length() const
{
	return string_block()->length;
}

template <typename Ops>
const char *string_object<Ops>::data() const
{
	return string_block()->data;
}

template <typename Ops>
StringBlock *string_object<Ops>::string_block() const
{
	return static_cast<StringBlock *> (object<Ops>::object_block());
}

} // namespace

#endif
