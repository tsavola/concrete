/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "concrete.hpp"

#include <iostream>

#include <boost/format.hpp>

#include <concrete/internals.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/string.hpp>

namespace concrete {

static Object CONCRETE_INTERNAL(ConcreteModule_test)(const TupleObject &args, const DictObject &kwargs)
{
	std::cout << boost::format("concrete.test: args=%d kwargs=%d")
		% args.size() % kwargs.size() << std::endl;

	return args.get_item(0);
}

void ConcreteModule::Init(const DictObject &modules) throw (AllocError)
{
	auto dict = DictObject::New(1);

	dict.set_item(
		StringObject::New("test"),
		InternalObject::New(internals::ConcreteModule_test));

	modules.set_item(StringObject::New("concrete"), ModuleObject::New(dict));
}

} // namespace
