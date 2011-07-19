/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "long-data.hpp"

#include <functional>

#include <boost/format.hpp>

#include <concrete/context-data.hpp>
#include <concrete/objects/bool.hpp>
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

	auto repr = InternalObject::New(internal::LongType_Repr);
	auto str  = InternalObject::New(internal::LongType_Str);
	auto lt   = InternalObject::New(internal::LongType_LT);
	auto le   = InternalObject::New(internal::LongType_LE);
	auto eq   = InternalObject::New(internal::LongType_EQ);
	auto ne   = InternalObject::New(internal::LongType_NE);
	auto gt   = InternalObject::New(internal::LongType_GT);
	auto ge   = InternalObject::New(internal::LongType_GE);

	auto add  = InternalObject::New(internal::LongType_Add);

	type.set(&PortableObjectProtocol::repr, repr);
	type.set(&PortableObjectProtocol::str,  str);
	type.set(&PortableObjectProtocol::lt,   lt);
	type.set(&PortableObjectProtocol::le,   le);
	type.set(&PortableObjectProtocol::eq,   eq);
	type.set(&PortableObjectProtocol::ne,   ne);
	type.set(&PortableObjectProtocol::gt,   gt);
	type.set(&PortableObjectProtocol::ge,   ge);

	type.set(&PortableObjectProtocol::add,  add);
}

static Object LongStr(const TupleObject &args, const DictObject &kwargs)
{
	return StringObject::New(
		(boost::format("%ld") % args.get_item(0).require<LongObject>().value()).str());
}

template <typename Predicate>
static Object LongCompare(const TupleObject &args, const DictObject &kwargs)
{
	return BoolObject::FromBool(Predicate()(
		args.get_item(0).require<LongObject>().value(),
		args.get_item(1).require<LongObject>().value()));
}

static Object LongAdd(const TupleObject &args, const DictObject &kwargs)
{
	LongObject::Value value = 0;

	for (unsigned int i = args.size(); i-- > 0; )
		value += args.get_item(i).require<LongObject>().value();

	return LongObject::New(value);
}

} // namespace

CONCRETE_INTERNAL_FUNCTION(LongType_Repr, LongStr)
CONCRETE_INTERNAL_FUNCTION(LongType_Str,  LongStr)
CONCRETE_INTERNAL_FUNCTION(LongType_LT,   LongCompare<std::less<LongObject::Value>>)
CONCRETE_INTERNAL_FUNCTION(LongType_LE,   LongCompare<std::less_equal<LongObject::Value>>)
CONCRETE_INTERNAL_FUNCTION(LongType_EQ,   LongCompare<std::equal_to<LongObject::Value>>)
CONCRETE_INTERNAL_FUNCTION(LongType_NE,   LongCompare<std::not_equal_to<LongObject::Value>>)
CONCRETE_INTERNAL_FUNCTION(LongType_GT,   LongCompare<std::greater<LongObject::Value>>)
CONCRETE_INTERNAL_FUNCTION(LongType_GE,   LongCompare<std::greater_equal<LongObject::Value>>)
CONCRETE_INTERNAL_FUNCTION(LongType_Add,  LongAdd)
