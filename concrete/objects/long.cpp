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

#include <concrete/internals.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

CONCRETE_INTERNAL(LongObject_add)(const TupleObject &args, const DictObject &kwargs)
{
	int64_t value = 0;

	for (unsigned int i = args.size(); i-- > 0; )
		value += args.get_item(i).require<LongObject>().value();

	return LongObject::New(value);
}

CONCRETE_INTERNAL_FUNCTION(LongObject_str)(const TupleObject &args, const DictObject &kwargs)
{
	return StringObject::New(
		(boost::format("%ld") % args.get_item(0).require<LongObject>().value()).str());
}

CONCRETE_INTERNAL_REGISTER(LongObject_repr, LongObject_str)
CONCRETE_INTERNAL_REGISTER(LongObject_str,  LongObject_str)

void LongInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("long"));

	type.protocol().add   = InternalObject::New(internals::LongObject_add);
	type.protocol().repr  = InternalObject::New(internals::LongObject_repr);
	type.protocol().str   = InternalObject::New(internals::LongObject_str);
}

} // namespace
