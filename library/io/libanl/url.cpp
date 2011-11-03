/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <library/io/url.hpp>

#include <library/io/libanl/addrinfo.hpp>

namespace concrete {

LibraryURLOpener::State LibraryURLOpener::resolve()
{
	m_addrinfo.reset(new AsyncAddrInfo(m_url.host(), m_url.port()));

	return resolving();
}

} // namespace
