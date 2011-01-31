/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_MODULES_CONCRETE_HPP
#define CONCRETE_MODULES_CONCRETE_HPP

#include <concrete/block.hpp>
#include <concrete/context.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/module.hpp>

namespace concrete {

struct ConcreteModule {
	static void RegisterInternals();
	static void Init(const DictObject &modules) throw (AllocError);
};

} // namespace

#endif
