/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_ERROR_HPP
#define CONCRETE_ERROR_HPP

#include <exception>

namespace concrete {

class KeyError: public std::exception {
public:
	const char *what() const throw ()
	{
		return "key not found";
	}
};

} // namespace

#endif
