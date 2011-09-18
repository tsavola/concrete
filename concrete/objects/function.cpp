/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "function-data.hpp"

#include <concrete/context-data.hpp>
#include <concrete/execution.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

CONCRETE_CONTINUATION_DEFAULT_IMPL(FunctionContinuation)

FunctionContinuation::Data::~Data() throw ()
{
	frame->destroy();
}

bool FunctionContinuation::initiate(Object &result,
                                    const TupleObject &args,
                                    const DictObject &kwargs,
                                    const CodeObject &code) const
{
	auto dict = DictObject::NewWithCapacity(args.size() + kwargs.size());

	for (unsigned int i = 0; i < args.size(); i++)
		dict.set_item(code.varnames().get_item(i), args.get_item(i));

	kwargs.copy_to(dict);

	auto frame = Context::Active().execution()->new_frame(code, dict);
	data()->frame = frame;

	return false;
}

bool FunctionContinuation::resume(Object &result, const CodeObject &code) const
{
	result = data()->frame->result();

	return true;
}

CONCRETE_OBJECT_DEFAULT_IMPL(FunctionObject, function_type)

FunctionObject::Data::Data(const TypeObject &type, const CodeObject &code) throw ():
	CallableObject::Data(type),
	code(code)
{
}

Object FunctionObject::continuable_call(Continuation &cont,
                                        Continuation::Stage stage,
                                        const TupleObject *args,
                                        const DictObject *kwargs) const
{
	CodeObject code = data()->code;
	return Continuation::Call<FunctionContinuation>(cont, stage, args, kwargs, code);
}

FunctionObject FunctionObject::New(const CodeObject &code)
{
	return NewObject<FunctionObject>(code);
}

void FunctionObjectTypeInit(const TypeObject &type, const char *name)
{
	CallableObjectTypeInit(type, name);
}

} // namespace
