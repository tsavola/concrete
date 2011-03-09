/*
 * Copyright (c) 2004-2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifdef CONCRETE_BACKTRACE

#include "backtrace.hpp"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include <cxxabi.h>
#include <execinfo.h>

#include <boost/format.hpp>

namespace concrete {

void print_backtrace()
{
	size_t size = 64;
	void *buffer[size];

	size = backtrace(buffer, size);
	auto strings = backtrace_symbols(buffer, size);

	std::cerr << "Backtrace:\n";

	for (size_t i = 0; i < size; i++) {
		auto beg = std::strchr(strings[i], '(');
		if (beg == NULL)
			continue;

		*beg = '\0';
		beg++;

		auto end = std::strchr(beg, '+');
		if (end == NULL) {
			end = std::strrchr(beg, ')');
			if (end == NULL)
				continue;
		}

		*end = '\0';

		auto sym = abi::__cxa_demangle(beg, 0, 0, 0);
		if (sym) {
			std::cerr << boost::format("  %s in %s\n") % sym % strings[i];
			std::free(sym);
		} else {
			std::cerr << boost::format("  %s in %s\n") % beg % strings[i];
		}
	}
}

} // namespace

#endif
