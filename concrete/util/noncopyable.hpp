/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_NONCOPYABLE_HPP
#define CONCRETE_UTIL_NONCOPYABLE_HPP

namespace concrete {

class noncopyable {
public:
	noncopyable()
	{
	}

private:
	noncopyable(const noncopyable &);
	noncopyable &operator=(const noncopyable &);
};

} // namespace

#endif
