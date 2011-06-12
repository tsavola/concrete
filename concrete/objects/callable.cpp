/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "callable-content.hpp"

#include <concrete/objects/function-content.hpp>
#include <concrete/objects/internal-content.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

CallableObject::Content::Content(const TypeObject &type):
	Object::Content(type)
{
}

CallableObject::CallableObject(BlockId id) throw ():
	Object(id)
{
}

CallableObject::CallableObject(const CallableObject &other) throw ():
	Object(other)
{
}

CallableObject &CallableObject::operator=(const CallableObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

Object CallableObject::init_call(BlockId &continuation,
                                 const TupleObject &args,
                                 const DictObject &kwargs) const
{
	return call(InitContinuation, continuation, &args, &kwargs);
}

Object CallableObject::resume_call(BlockId &continuation) const
{
	return call(ResumeContinuation, continuation);
}

void CallableObject::cleanup_call(BlockId continuation) const throw ()
{
	try {
		call(CleanupContinuation, continuation);
	} catch (...) {
		assert(false);
	}
}

Object CallableObject::call(ContinuationOp op,
                            BlockId &continuation,
                            const TupleObject *args,
                            const DictObject *kwargs) const
{
	TypeObject derived_type = type();
	Object value;

	if (derived_type == FunctionObject::Type())
		value = static_cast<const FunctionObject::Content *> (content())->call(
			op, continuation, args, kwargs);

	else if (derived_type == InternalObject::Type())
		value = static_cast<const InternalObject::Content *> (content())->call(
			op, continuation, args, kwargs);

	return value;
}

CallableObject::Content *CallableObject::content() const
{
	return content_pointer<Content>();
}

bool TypeCheck<CallableObject>::operator()(const TypeObject &type)
{
	return type == FunctionObject::Type() || type == InternalObject::Type();
}

} // namespace
