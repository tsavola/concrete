/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "tuple-content.hpp"

#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void TupleObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("tuple"));

	Context::SystemObjects()->tuple_empty = TupleObject::NewWithSize(0);
}

TupleObject::Content::Content(const TypeObject &type):
	Object::Content(type)
{
	for (unsigned int i = 0; i < size(); i++)
		new (&items[i]) Portable<Object>();
}

TupleObject::Content::~Content() throw ()
{
	for (unsigned int i = size(); i-- > 0; )
		items[i].~Portable<Object>();
}

unsigned int TupleObject::Content::size() const throw ()
{
	return (block_size() - sizeof (Content)) / sizeof (Portable<Object>);
}

TypeObject TupleObject::Type()
{
	return Context::SystemObjects()->tuple_type;
}

TupleObject TupleObject::New()
{
	return Context::SystemObjects()->tuple_empty->cast<TupleObject>();
}

TupleObject TupleObject::NewWithSize(unsigned int size)
{
	return Context::NewCustomSizeBlock<Content>(
		sizeof (Content) + sizeof (Portable<Object>) * size,
		Type());
}

TupleObject::TupleObject(BlockId id) throw ():
	Object(id)
{
}

TupleObject::TupleObject(const TupleObject &other) throw ():
	Object(other)
{
}

TupleObject &TupleObject::operator=(const TupleObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

void TupleObject::init_item(unsigned int index, const Object &item)
{
	auto c = content();
	assert(index < c->size());
	c->items[index] = item;
}

unsigned int TupleObject::size() const
{
	return content()->size();
}

Object TupleObject::get_item(unsigned int index) const
{
	auto c = content();
	if (index >= c->size())
		throw RuntimeError("tuple index out of bounds");
	return c->items[index];
}

TupleObject::Content *TupleObject::content() const
{
	return content_pointer<Content>();
}

} // namespace
