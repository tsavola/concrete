/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "long-data.hpp"

#include <boost/format.hpp>

#include <concrete/context-data.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

LongObject::Data::Data(const TypeObject &type, Value value):
	Object::Data(type),
	value(value)
{
}

TypeObject LongObject::Type()
{
	return Context::Active().data()->long_type;
}

LongObject LongObject::New(Value value)
{
	return NewObject<LongObject>(value);
}

LongObject::Value LongObject::value() const
{
	return data()->value;
}

LongObject::Data *LongObject::data() const
{
	return data_cast<Data>();
}

void LongObjectTypeInit(const TypeObject &type, const char *name)
{
	ObjectTypeInit(type, name);

	type.protocol()->repr  = InternalObject::New(internal::LongType_Repr);
	type.protocol()->str   = InternalObject::New(internal::LongType_Str);

	type.protocol()->add   = InternalObject::New(internal::LongType_Add);
}

static Object LongAdd(const TupleObject &args, const DictObject &kwargs)
{
	LongObject::Value value = 0;

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
