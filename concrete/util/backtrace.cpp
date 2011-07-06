/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifdef CONCRETE_BACKTRACE

#include "backtrace.hpp"

#include <cstdlib>
#include <cstring>
#include <iostream>

#include <unistd.h>

#include <boost/format.hpp>

#include <concrete/util/trace.hpp>

namespace concrete {

void Backtrace() throw ()
{
	std::string command = (boost::format("gdb -batch -ex bt /proc/%1%/exe %1%") % getpid()).str();

	if (std::system(command.c_str()))
		Trace("Backtrace() failed to run \"%s\"", command);
}

} // namespace

#endif
