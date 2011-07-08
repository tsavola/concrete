/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "dict-data.hpp"

#include <algorithm>
#include <cassert>

#include <concrete/arena.hpp>
#include <concrete/context-data.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/portable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

DictObject::Data::Item::Item(const Object &key, const Object &value):
	key(key),
	value(value)
{
}

DictObject::Data::Data(const TypeObject &type):
	Object::Data(type)
{
}

DictObject::Data::~Data() throw ()
{
	for (unsigned int i = std::min(uint32_t(size), capacity()); i-- > 0; )
		items[i].~Item();
}

unsigned int DictObject::Data::capacity() const throw ()
{
	return (Arena::AllocationSize(this) - sizeof (Data)) / sizeof (Item);
}

unsigned int DictObject::Data::find_item(const Object &key) const
{
	unsigned int i;

	for (i = 0; i < size; i++)
		if (key.cast<StringObject>().equals(items[i].key->cast<StringObject>()))
			break;

	return i;
}

TypeObject DictObject::Type()
{
	return Context::Active().data()->dict_type;
}

DictObject DictObject::NewEmpty()
{
	return NewWithCapacity(0);
}

DictObject DictObject::New()
{
	return NewWithCapacity(16);
}

DictObject DictObject::NewWithCapacity(unsigned int capacity)
{
	return NewCustomSizeObject<DictObject>(sizeof (Data) + sizeof (Data::Item) * capacity);
}

unsigned int DictObject::size() const
{
	return data()->size;
}

void DictObject::set_item(const Object &key, const Object &value) const
{
	key.require<StringObject>();

	auto i = data()->find_item(key);

	if (i < data()->size) {
		data()->items[i].value = value;
		return;
	}

	if (i == data()->capacity())
		throw RuntimeError("dict capacity exceeded");

	new (&data()->items[i]) Data::Item(key, value);
	data()->size = i + 1;
}

Object DictObject::get_item(const Object &key) const
{
	Object value;

	if (!get_item(key, value))
		throw KeyError(key);

	return value;
}

bool DictObject::get_item(const Object &key, Object &value) const
{
	if (!key.check<StringObject>())
		return false;

	auto i = data()->find_item(key);

	if (i == data()->size)
		return false;

	value = data()->items[i].value;
	return true;
}

void DictObject::copy_to(const DictObject &target) const
{
	for (unsigned int i = 0; i < size(); i++)
		target.set_item(data()->items[i].key, data()->items[i].value);
}

unsigned int DictObject::capacity() const
{
	return data()->capacity();
}

DictObject::Data *DictObject::data() const
{
	auto c = data_cast<Data>();

	if (*c->size > c->capacity())
		throw IntegrityError(address());

	return c;
}

void DictObjectTypeInit(const TypeObject &type, const char *name)
{
	ObjectTypeInit(type, name);
}

} // namespace
