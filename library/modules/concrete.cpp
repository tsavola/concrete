/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <iostream>

#include <boost/format.hpp>

#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>

namespace concrete {

Object ConcreteModule_Test(const TupleObject &args, const DictObject &kwargs)
{
	std::cout << boost::format("concrete.test: args=%d kwargs=%d")
		% args.size() % kwargs.size() << std::endl;

	return args.get_item(0);
}

} // namespace
