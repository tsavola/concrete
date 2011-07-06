/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIB_API_HPP
#define LIB_API_HPP

#define CONCRETE_API __attribute__ ((visibility ("default")))

#include "concrete.h"

#include <memory>

#include <concrete/context.hpp>
#include <lib/libevent.hpp>

struct ConcreteContext {
	std::unique_ptr<concrete::LibeventLoop> event_loop;
	std::unique_ptr<concrete::Context>      context;
};

#endif
