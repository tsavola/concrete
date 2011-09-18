/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "builtins.hpp"

#include <iostream>

#include <concrete/nested-data.hpp>
#include <concrete/objects/bool.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/string.hpp>

namespace concrete {

DictObject BuiltinsModuleInit(const DictObject &modules)
{
	auto dict = DictObject::New();

	dict.set_item(StringObject::New("None"),  Object());
	dict.set_item(StringObject::New("True"),  BoolObject::True());
	dict.set_item(StringObject::New("False"), BoolObject::False());

	dict.set_item(StringObject::New("id"),    InternalObject::New(internal::BuiltinsModule_Id));
	dict.set_item(StringObject::New("print"), InternalObject::New(internal::BuiltinsModule_Print));
	dict.set_item(StringObject::New("repr"),  InternalObject::New(internal::BuiltinsModule_Repr));
	dict.set_item(StringObject::New("str"),   InternalObject::New(internal::BuiltinsModule_Str));
	dict.set_item(StringObject::New("len"),   InternalObject::New(internal::BuiltinsModule_Len));

	modules.set_item(StringObject::New("builtins"), ModuleObject::New(dict));

	return dict;
}

static Object Id(const TupleObject &args, const DictObject &kwargs)
{
	return LongObject::New(args.get_item(0).address());
}

class Print: public NestedContinuation {
	CONCRETE_CONTINUATION_DEFAULT_DECL(Print, NestedContinuation)

protected:
	struct Data: NestedContinuation::Data {
		Portable<uint32_t> index;
		Portable<Object>   args;
		Portable<Object>   deferred_value;
	} CONCRETE_PACKED;

public:
	bool initiate(Object &result, const TupleObject &args, const DictObject &kwargs) const
	{
		if (args.size() == 0)
			return finish();

		data()->index = 0;
		data()->args = args;

		return evaluate();
	}

	bool resume(Object &result) const
	{
		Object value;

		if (in_nested_call()) {
			if (!resume_nested(value))
				return false;
		} else {
			value = data()->deferred_value;
		}

		std::cout << value.require<StringObject>().string();

		unsigned int index = data()->index;
		unsigned int size = data()->args->cast<TupleObject>().size();

		if (index == size - 1)
			return finish();

		std::cout << " ";

		data()->index = ++index;

		return evaluate();
	}

private:
	bool evaluate() const
	{
		unsigned int index = data()->index;
		auto self = data()->args->cast<TupleObject>().get_item(index);
		auto callable = self.protocol()->str->require<CallableObject>();

		Object value;
		auto tuple = TupleObject::New(self);
		auto dict = DictObject::NewEmpty();

		if (call_nested(callable, value, tuple, dict))
			data()->deferred_value = value;

		return false;
	}

	bool finish() const
	{
		std::cout << std::endl;

		return true;
	}
};

CONCRETE_CONTINUATION_DEFAULT_IMPL(Print)

static Object Repr(const TupleObject &args, const DictObject &kwargs)
{
	return args.get_item(0).repr();
}

static NestedCall Str(const TupleObject &args, const DictObject &kwargs)
{
	return NestedCall(args.get_item(0).protocol()->repr, args, kwargs);
}

static NestedCall Len(const TupleObject &args, const DictObject &kwargs)
{
	return NestedCall(args.get_item(0).protocol()->len, args, kwargs);
}

} // namespace

CONCRETE_INTERNAL_FUNCTION    (BuiltinsModule_Id,    Id)
CONCRETE_INTERNAL_CONTINUATION(BuiltinsModule_Print, Print)
CONCRETE_INTERNAL_FUNCTION    (BuiltinsModule_Repr,  Repr)
CONCRETE_INTERNAL_NESTED_CALL (BuiltinsModule_Str,   Str)
CONCRETE_INTERNAL_NESTED_CALL (BuiltinsModule_Len,   Len)
