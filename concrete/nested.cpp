/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "nested-data.hpp"

namespace concrete {

CONCRETE_POINTER_IMPL_DATA(NestedContinuation)

NestedContinuation::Data::~Data() throw ()
{
	if (*nested)
		callable->cast<CallableObject>().release(nested);
}

bool NestedContinuation::call_nested(const CallableObject &callable,
                                     Object &result,
                                     const TupleObject &args,
                                     const DictObject &kwargs) const
{
	data()->callable = callable;

	Continuation nested;
	result = callable.initiate(nested, args, kwargs);
	data()->nested = nested;

	return !nested;
}

bool NestedContinuation::in_nested_call() const
{
	return *data()->nested;
}

bool NestedContinuation::resume_nested(Object &result) const
{
	auto callable = data()->callable->cast<CallableObject>();

	Continuation nested = data()->nested;
	result = callable.resume(nested);
	data()->nested = nested;

	return !nested;
}

NestedCall::NestedCall(const Object &callable):
	callable(callable.require<CallableObject>()),
	args    (TupleObject::New()),
	kwargs  (DictObject::NewEmpty())
{
}

NestedCall::NestedCall(const Object &callable, const TupleObject &args):
	callable(callable.require<CallableObject>()),
	args    (args),
	kwargs  (DictObject::NewEmpty())
{
}

NestedCall::NestedCall(const Object &callable, const TupleObject &args, const DictObject &kwargs):
	callable(callable.require<CallableObject>()),
	args    (args),
	kwargs  (kwargs)
{
}

NestedCall::NestedCall(const NestedCall &other) throw ():
	callable(other.callable),
	args    (other.args),
	kwargs  (other.kwargs)
{
}

bool NestedCallContinuation::initiate(Object &result,
                                      const TupleObject &args,
                                      const DictObject &kwargs,
                                      DeclaratorFunction declare) const
{
	auto call = declare(args, kwargs);
	return call_nested(call.callable, result, call.args, call.kwargs);
}

bool NestedCallContinuation::resume(Object &result, DeclaratorFunction declare) const
{
	return resume_nested(result);
}

} // namespace
