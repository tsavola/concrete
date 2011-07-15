/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_IO_ERROR_HPP
#define LIBRARY_IO_ERROR_HPP

#ifdef CONCRETE_TRACE
# include <cerrno>
# include <cstring>
#endif

#include <concrete/resource.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

class IOResourceError: public ResourceError {
public:
	explicit IOResourceError(int error = errno)
	{
#ifdef CONCRETE_TRACE
		Trace(std::strerror(error));
#endif
	}
};

} // namespace

#endif
