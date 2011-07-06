/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_IO_RESOLVE_HPP
#define CONCRETE_IO_RESOLVE_HPP

#include <string>

#include <concrete/resource.hpp>

struct addrinfo;

namespace concrete {

class Resolve: public Resource {
public:
	void suspend_until_resolved();
	struct addrinfo *addrinfo();
};

template <> struct ResourceCreate<Resolve> {
	static Resolve *New(const std::string &node, const std::string &service);
};

} // namespace

#endif
