/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "bytes-data.hpp"

#include <cstring>

#include <concrete/arena.hpp>
#include <concrete/context-data.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

BytesObject::Data::Data(const TypeObject &type, const uint8_t *data_):
	Object::Data(type)
{
	std::memcpy(data, data_, size());
}

size_t BytesObject::Data::size() const throw ()
{
	return Arena::AllocationSize(this) - sizeof (Data);
}

TypeObject BytesObject::Type()
{
	return Context::Active().data()->bytes_type;
}

BytesObject BytesObject::New(const uint8_t *data, size_t size)
{
	return NewCustomSizeObject<BytesObject>(sizeof (Data) + size, data);
}

size_t BytesObject::size() const
{
	return data()->size();
}

const uint8_t *BytesObject::c_data() const
{
	return data()->data;
}

BytesObject::Data *BytesObject::data() const
{
	return data_cast<Data>();
}

void BytesObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("bytes"));
}

} // namespace
