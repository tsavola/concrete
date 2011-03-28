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

#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/util/nested.hpp>

namespace concrete {

DictObject BuiltinsModuleInit(const DictObject &modules)
{
	auto dict = DictObject::New();

	dict.set_item(StringObject::New("None"),  Context::SystemObjects()->none);

	dict.set_item(StringObject::New("id"),    InternalObject::New(internal::BuiltinsModule_Id));
	dict.set_item(StringObject::New("print"), InternalObject::New(internal::BuiltinsModule_Print));
	dict.set_item(StringObject::New("repr"),  InternalObject::New(internal::BuiltinsModule_Repr));
	dict.set_item(StringObject::New("str"),   InternalObject::New(internal::BuiltinsModule_Str));

	modules.set_item(StringObject::New("builtins"), ModuleObject::New(dict));

	return dict;
}

static Object Id(const TupleObject &args, const DictObject &kwargs)
{
	return LongObject::New(args.get_item(0).id().offset());
}

class PrintContinuation: public NestedContinuation {
public:
	Portable<uint32_t> index;
	PortableObject args;

	PortableObject deferred_value;

} CONCRETE_PACKED;

struct Print: NestedContinuable {
	bool call(BlockId state_id, Object &, const TupleObject &args, const DictObject &kwargs) const
	{
		if (args.size() == 0)
			return finish();

		PrintState(state_id)->index = 0;
		PrintState(state_id)->args = args;

		return evaluate(state_id);
	}

	bool resume(BlockId state_id, Object &) const
	{
		Object value;

		if (in_nested_call(state_id)) {
			if (!resume_nested(state_id, value))
				return false;
		} else {
			value = PrintState(state_id)->deferred_value;
		}

		std::cout << value.require<StringObject>().string();

		unsigned int index = PrintState(state_id)->index;
		unsigned int size = PrintState(state_id)->args.cast<TupleObject>().size();

		if (index == size - 1)
			return finish();

		std::cout << " ";

		PrintState(state_id)->index = ++index;

		return evaluate(state_id);
	}

private:
	bool evaluate(BlockId state_id) const
	{
		unsigned int index = PrintState(state_id)->index;
		auto self = PrintState(state_id)->args.cast<TupleObject>().get_item(index);
		auto callable = self.protocol().str.require<CallableObject>();

		Object value;
		auto tuple = TupleObject::New(self);
		auto dict = DictObject::EmptySingleton();

		if (call_nested(state_id, callable, value, tuple, dict))
			PrintState(state_id)->deferred_value = value;

		return false;
	}

	bool finish() const
	{
		std::cout << std::endl;

		return true;
	}

	static PrintContinuation *PrintState(BlockId id)
	{
		return static_cast<PrintContinuation *> (NestedState(id));
	}
};

static Object Repr(const TupleObject &args, const DictObject &kwargs)
{
	return args.get_item(0).repr();
}

static NestedCall Str(const TupleObject &args, const DictObject &kwargs)
{
	return NestedCall(args.get_item(0).protocol().repr, args, kwargs);
}

} // namespace

CONCRETE_INTERNAL_FUNCTION   (BuiltinsModule_Id,    Id)
CONCRETE_INTERNAL_CONTINUABLE(BuiltinsModule_Print, concrete::Print, concrete::PrintContinuation)
CONCRETE_INTERNAL_FUNCTION   (BuiltinsModule_Repr,  Repr)
CONCRETE_INTERNAL_NESTED_CALL(BuiltinsModule_Str,   Str)
