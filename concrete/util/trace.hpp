/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_TRACE_HPP
#define CONCRETE_UTIL_TRACE_HPP

#ifdef CONCRETE_TRACE

# include <iostream>
# include <boost/format.hpp>

# define ConcreteTrace(args) \
	do { \
		try { \
			std::cerr << "TRACE: " << (boost::format args) << std::endl; \
		} catch (...) {} \
	} while (0)

#else

# define ConcreteTrace(args) \
	do {} while (0)

#endif

#endif
