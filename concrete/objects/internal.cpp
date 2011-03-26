/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "internal.hpp"

#include <cassert>

#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

static InternalFunction *functions[] = {
#	define CONCRETE_INTERNAL_SYMBOL(Symbol) internal_symbol::Symbol,
#	include <concrete/internals.hpp>
#	undef CONCRETE_INTERNAL_SYMBOL
};

void InternalTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("internal"));
}

Object InternalBlock::call(ContinuationOp op,
                           BlockId &continuation,
                           const TupleObject *args,
                           const DictObject *kwargs) const
{
	unsigned int index = symbol_id;

	if (index >= sizeof (functions) / sizeof (functions[0]))
		throw RuntimeError("internal call undefined");

	assert(functions[index]);
	return functions[index](op, continuation, args, kwargs);
}

} // namespace
