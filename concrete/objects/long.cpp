/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "long-content.hpp"

#include <boost/format.hpp>

#include <concrete/context.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void LongObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("long"));

	type.protocol().add   = InternalObject::New(internal::LongType_Add);
	type.protocol().repr  = InternalObject::New(internal::LongType_Repr);
	type.protocol().str   = InternalObject::New(internal::LongType_Str);
}

LongObject::Content::Content(const TypeObject &type, int64_t value):
	Object::Content(type),
	value(value)
{
}

TypeObject LongObject::Type()
{
	return Context::SystemObjects()->long_type;
}

LongObject LongObject::New(int64_t value)
{
	return Context::NewBlock<Content>(Type(), value);
}

LongObject::LongObject(BlockId id) throw ():
	Object(id)
{
}

LongObject::LongObject(const LongObject &other) throw ():
	Object(other)
{
}

LongObject &LongObject::operator=(const LongObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

int64_t LongObject::value() const
{
	return content()->value;
}

LongObject::Content *LongObject::content() const
{
	return content_pointer<Content>();
}

static Object LongAdd(const TupleObject &args, const DictObject &kwargs)
{
	int64_t value = 0;

	for (unsigned int i = args.size(); i-- > 0; )
		value += args.get_item(i).require<LongObject>().value();

	return LongObject::New(value);
}

static Object LongStr(const TupleObject &args, const DictObject &kwargs)
{
	return StringObject::New(
		(boost::format("%ld") % args.get_item(0).require<LongObject>().value()).str());
}

} // namespace

CONCRETE_INTERNAL_FUNCTION(LongType_Add,  LongAdd)
CONCRETE_INTERNAL_FUNCTION(LongType_Repr, LongStr)
CONCRETE_INTERNAL_FUNCTION(LongType_Str,  LongStr)
