/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "bytes-content.hpp"

#include <cstring>

#include <concrete/context.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void BytesObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("bytes"));
}

BytesObject::Content::Content(const TypeObject &type, const uint8_t *data_):
	Object::Content(type)
{
	std::memcpy(data, data_, size());
}

size_t BytesObject::Content::size() const throw ()
{
	return block_size() - sizeof (Object::Content);
}

TypeObject BytesObject::Type()
{
	return Context::SystemObjects()->bytes_type;
}

BytesObject BytesObject::New(const uint8_t *data, size_t size)
{
	return Context::NewCustomSizeBlock<Content>(sizeof (Object::Content) + size, Type(), data);
}

BytesObject::BytesObject(BlockId id) throw ():
	Object(id)
{
}

BytesObject::BytesObject(const BytesObject &other) throw ():
	Object(other)
{
}

BytesObject &BytesObject::operator=(const BytesObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

size_t BytesObject::size() const
{
	return content()->size();
}

const uint8_t *BytesObject::data() const
{
	return content()->data;
}

BytesObject::Content *BytesObject::content() const
{
	return content_pointer<Content>();
}

} // namespace
