/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "bool.hpp"

#include <concrete/context-data.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/long-data.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

TypeObject BoolObject::Type()
{
	return Context::Active().data()->bool_type;
}

BoolObject BoolObject::True()
{
	return Context::Active().data()->bool_true->cast<BoolObject>();
}

BoolObject BoolObject::False()
{
	return Context::Active().data()->bool_false->cast<BoolObject>();
}

BoolObject BoolObject::FromBool(bool value)
{
	return value ? True() : False();
}

BoolObject BoolObject::NewBuiltin(bool value)
{
	return NewObject<BoolObject>(value);
}

void BoolObjectTypeInit(const TypeObject &type, const char *name)
{
	LongObjectTypeInit(type, name);

	type.protocol()->repr  = InternalObject::New(internal::BoolType_Repr);
	type.protocol()->str   = InternalObject::New(internal::BoolType_Str);

	Context::Active().data()->bool_true  = BoolObject::NewBuiltin(true);
	Context::Active().data()->bool_false = BoolObject::NewBuiltin(false);
}

static Object BoolStr(const TupleObject &args, const DictObject &kwargs)
{
	return StringObject::New(args.get_item(0).require<BoolObject>().value() ? "True" : "False");
}

} // namespace

CONCRETE_INTERNAL_FUNCTION(BoolType_Repr, BoolStr)
CONCRETE_INTERNAL_FUNCTION(BoolType_Str,  BoolStr)
