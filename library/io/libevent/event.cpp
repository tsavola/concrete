/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cstddef>

#include <evdns.h>

#include <concrete/resource.hpp>

#include <library/libevent/event.hpp>

namespace concrete {

struct evdns_base *LibeventLoop::new_evdns_base()
{
	auto evdns_base = evdns_base_new(m_base, true);
	if (evdns_base == NULL)
		throw ResourceError();

	return evdns_base;
}

} // namespace
