/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifdef CONCRETE_BACKTRACE

# include <cstdlib>
# include <iostream>

# include <concrete/util/backtrace.hpp>

# ifdef assert
# undef assert
# endif

# define assert(expr) \
	do { \
		if (!(expr)) { \
			std::cerr << __FILE__ << ":" << __LINE__ << ": assertion failed: " \
			          << #expr << std::endl; \
			concrete::Backtrace(); \
			std::abort(); \
		} \
	} while (0)

#else

# include <cassert>

#endif
