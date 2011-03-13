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
	Portable<uint32_t> length;
	char data[0];

	StringBlock(const TypeObject &type, const char *data);

	size_t size() const throw ()
	{
		return block_size() - sizeof (StringBlock) - 1;
	}
} CONCRETE_PACKED;

template <typename Ops>
TypeObject StringLogic<Ops>::Type()
{
	return Context::BuiltinObjects().string_type;
}

template <typename Ops>
StringLogic<Ops> StringLogic<Ops>::New(const char *data, size_t size)
{
	return Context::NewCustomSizeBlock<StringBlock>(sizeof (StringBlock) + size + 1, Type(), data);
}

template <typename Ops>
size_t StringLogic<Ops>::size() const
{
	return string_block()->size();
}

template <typename Ops>
size_t StringLogic<Ops>::length() const
{
	return string_block()->length;
}

template <typename Ops>
const char *StringLogic<Ops>::data() const
{
	return string_block()->data;
}

template <typename Ops>
StringBlock *StringLogic<Ops>::string_block() const
{
	return static_cast<StringBlock *> (ObjectLogic<Ops>::object_block());
}

void StringInit(const TypeObject &type);

} // namespace

#endif
