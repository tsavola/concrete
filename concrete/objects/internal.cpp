/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "internal-content.hpp"

#include <cassert>

#include <concrete/context.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void InternalObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("internal"));
}

static InternalFunction *functions[] = {
#	define CONCRETE_INTERNAL_SYMBOL(Symbol) internal_symbol::Symbol,
#	include <concrete/internals.hpp>
#	undef CONCRETE_INTERNAL_SYMBOL
};

InternalObject::Content::Content(const TypeObject &type, internal::SymbolId symbol_id):
	CallableObject::Content(type),
	symbol_id(symbol_id)
{
}

Object InternalObject::Content::call(ContinuationOp op,
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

TypeObject InternalObject::Type()
{
	return Context::SystemObjects()->internal_type;
}

InternalObject InternalObject::New(internal::SymbolId symbol_id)
{
	return Context::NewBlock<Content>(Type(), symbol_id);
}

InternalObject::InternalObject(BlockId id) throw ():
	CallableObject(id)
{
}

InternalObject::InternalObject(const InternalObject &other) throw ():
	CallableObject(other)
{
}

InternalObject &InternalObject::operator=(const InternalObject &other) throw ()
{
	CallableObject::operator=(other);
	return *this;
}

Object InternalObject::call_args(const TupleObject &args) const
{
	BlockId continuation = 0;
	auto kwargs = DictObject::EmptySingleton();
	auto value = content()->call(InitContinuation, continuation, &args, &kwargs);

	if (continuation) {
		content()->call(CleanupContinuation, continuation, NULL, NULL);
		throw RuntimeError("non-trivial protocol method called from native code");
	}

	return value;
}

InternalObject::Content *InternalObject::content() const
{
	return content_pointer<Content>();
}

} // namespace
