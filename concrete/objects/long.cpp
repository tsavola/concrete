/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "long.hpp"

#include <stdexcept>

#include <concrete/internals.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

static Object add(const TupleObject &args, const DictObject &kwargs)
{
	if (kwargs.size() > 0)
		throw std::runtime_error("unexpected keyword arguments");

	if (args.size() != 2)
		throw std::runtime_error("two arguments expected");

	auto a = args.get_item(0).require<LongObject>("long object expected");
	auto b = args.get_item(1).require<LongObject>("long object expected");

	auto r = LongObject::New(a.value() + b.value());

	concrete_trace(("LongObject.add: %d + %d = %d") % a.value() % b.value() % r.value());

	return r;
}

void LongType::RegisterInternals()
{
	InternalObject::Register(internals::LongObject_add, add);
}

void LongType::Init(const TypeObject &type)
{
	type.init_builtin(StringObject::New("long"));

	type.protocol().add = InternalObject::New(internals::LongObject_add);
}

} // namespace
