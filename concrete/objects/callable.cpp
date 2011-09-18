/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "callable.hpp"

#include <concrete/objects/function-data.hpp>
#include <concrete/objects/internal-data.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/util/assert.hpp>

namespace concrete {

CONCRETE_OBJECT_MINIMAL_IMPL(CallableObject)

Object CallableObject::initiate(Continuation &cont,
                                const TupleObject &args,
                                const DictObject &kwargs) const
{
	return continuable_call(cont, Continuation::Initiate, &args, &kwargs);
}

Object CallableObject::resume(Continuation &cont) const
{
	return continuable_call(cont, Continuation::Resume);
}

void CallableObject::release(Continuation cont) const throw ()
{
	try {
		continuable_call(cont, Continuation::Release);
	} catch (...) {
		assert(false);
	}
}

Object CallableObject::continuable_call(Continuation &cont,
                                        Continuation::Stage stage,
                                        const TupleObject *args,
                                        const DictObject *kwargs) const
{
	TypeObject derived_type = type();
	Object value;

	if (derived_type == FunctionObject::Type())
		value = cast<FunctionObject>().continuable_call(cont, stage, args, kwargs);

	else if (derived_type == InternalObject::Type())
		value = cast<InternalObject>().continuable_call(cont, stage, args, kwargs);

	return value;
}

bool TypeCheck<CallableObject>::operator()(const TypeObject &type)
{
	return type == FunctionObject::Type() || type == InternalObject::Type();
}

void CallableObjectTypeInit(const TypeObject &type, const char *name)
{
	ObjectTypeInit(type, name);
}

} // namespace
