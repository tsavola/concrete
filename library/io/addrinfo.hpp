/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_IO_ADDRINFO_HPP
#define LIBRARY_IO_ADDRINFO_HPP

#include <concrete/util/noncopyable.hpp>

struct addrinfo;

namespace concrete {

class AddrInfo: Noncopyable {
public:
	virtual ~AddrInfo() throw () {}

	virtual struct addrinfo *resolved() = 0;
	virtual void suspend_until_resolved() = 0;
};

} // namespace

#endif
