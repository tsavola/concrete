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

#include <concrete/internals.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/string.hpp>

namespace concrete {

DictObject BuiltinsModuleInit(const DictObject &modules)
{
	auto dict = DictObject::New(3);

	dict.set_item(StringObject::New("print"), InternalObject::New(internals::BuiltinsModule_print));
	dict.set_item(StringObject::New("repr"),  InternalObject::New(internals::BuiltinsModule_repr));
	dict.set_item(StringObject::New("str"),   InternalObject::New(internals::BuiltinsModule_str));

	modules.set_item(StringObject::New("builtins"), ModuleObject::New(dict));

	return dict;
}

CONCRETE_INTERNAL(BuiltinsModule_print)(const TupleObject &args, const DictObject &kwargs)
{
	unsigned int size = args.size();

	for (unsigned int i = 0; i < size; i++) {
		std::cout << args.get_item(i).str().string();

		if (i < size - 1)
			std::cout << " ";
	}

	std::cout << std::endl;

	return Object();
}

CONCRETE_INTERNAL(BuiltinsModule_repr)(const TupleObject &args, const DictObject &kwargs)
{
	return args.get_item(0).repr();
}

CONCRETE_INTERNAL(BuiltinsModule_str)(const TupleObject &args, const DictObject &kwargs)
{
	return args.get_item(0).str();
}

} // namespace
