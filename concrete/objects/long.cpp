/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "long.hpp"

#include <boost/format.hpp>

#include <concrete/objects/internal.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void LongTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("long"));

	type.protocol().add   = InternalObject::New(internal::LongType_Add);
	type.protocol().repr  = InternalObject::New(internal::LongType_Repr);
	type.protocol().str   = InternalObject::New(internal::LongType_Str);
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
