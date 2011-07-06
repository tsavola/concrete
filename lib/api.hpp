/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETELIB_API_HPP
#define CONCRETELIB_API_HPP

#define CONCRETE_API __attribute__ ((visibility ("default")))

#include "concrete.h"

#include <concrete/context.hpp>

struct ConcreteContext {
	template <typename... Args>
	explicit ConcreteContext(Args... args): context(args...) {}

	concrete::Context context;
};

#endif
