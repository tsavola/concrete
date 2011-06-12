/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "dict-content.hpp"

#include <algorithm>
#include <cassert>

#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/object-content.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

void DictObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("dict"));

	Context::SystemObjects()->dict_empty = DictObject::NewWithCapacity(0);
}

DictObject::Content::Item::Item(const Object &key, const Object &value):
	key(key),
	value(value)
{
}

DictObject::Content::Content(const TypeObject &type):
	Object::Content(type)
{
}

DictObject::Content::~Content() throw ()
{
	for (unsigned int i = std::min(uint32_t(size), capacity()); i-- > 0; )
		items[i].~Item();
}

void DictObject::Content::verify_integrity() const
{
	if (uint32_t(size) > capacity())
		throw IntegrityError(this);
}

unsigned int DictObject::Content::capacity() const throw ()
{
	return (block_size() - sizeof (Content)) / sizeof (Item);
}

unsigned int DictObject::Content::find_item(const Object &key) const
{
	unsigned int i;

	for (i = 0; i < size; i++)
		if (key.cast<StringObject>().equals(items[i].key->cast<StringObject>()))
			break;

	return i;
}

TypeObject DictObject::Type()
{
	return Context::SystemObjects()->dict_type;
}

DictObject DictObject::EmptySingleton()
{
	auto empty = Context::SystemObjects()->dict_empty->cast<DictObject>();
	assert(empty.capacity() == 0);
	return empty;
}

DictObject DictObject::New()
{
	return NewWithCapacity(16);
}

DictObject DictObject::NewWithCapacity(unsigned int capacity)
{
	return Context::NewCustomSizeBlock<Content>(
		sizeof (Content) + sizeof (Content::Item) * capacity,
		Type());
}

DictObject::DictObject(BlockId id) throw ():
	Object(id)
{
}

DictObject::DictObject(const DictObject &other) throw ():
	Object(other)
{
}

DictObject &DictObject::operator=(const DictObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

unsigned int DictObject::size() const
{
	return content()->size;
}

void DictObject::set_item(const Object &key, const Object &value) const
{
	key.require<StringObject>();

	auto b = content();
	auto i = b->find_item(key);

	if (i < b->size) {
		b->items[i].value = value;
		return;
	}

	if (i == b->capacity())
		throw RuntimeError("dict capacity exceeded");

	new (&b->items[i]) Content::Item(key, value);
	b->size = i + 1;
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

	auto b = content();
	auto i = b->find_item(key);

	if (i == b->size)
		return false;

	value = b->items[i].value;
	return true;
}

void DictObject::copy_to(const DictObject &target) const
{
	for (unsigned int i = 0; i < size(); i++) {
		auto b = content();
		target.set_item(b->items[i].key, b->items[i].value);
	}
}

unsigned int DictObject::capacity() const
{
	return content()->capacity();
}

DictObject::Content *DictObject::content() const
{
	auto block = content_pointer<Content>();
	block->verify_integrity();
	return block;
}

} // namespace
