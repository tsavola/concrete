/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "url.hpp"

#include <cstring>

#include <concrete/exception.hpp>

namespace concrete {

URL::URL(const char *url)
{
	size_t len = std::strlen(url);
	if (len > MaxLength)
		throw RuntimeError("URL too long");

	const char *url_end = url + len;

	if (std::strncmp(url, "http://", std::strlen("http://")) != 0)
		throw RuntimeError("URL scheme not supported");

	const char *addr = url + std::strlen("http://");
	const char *addr_end = std::strchr(addr, '/');
	if (addr_end == NULL)
		addr_end = url_end;

	const char *host = addr;
	const char *host_end = std::strchr(addr, ':');
	if (host_end == NULL || host_end > addr_end)
		host_end = addr_end;

	if (host == host_end)
		throw RuntimeError("URL host is empty");

	m_host.assign(host, host_end - host);

	if (host_end == addr_end) {
		m_port = "80";
	} else {
		const char *port = host_end + 1;
		if (port == addr_end)
			throw RuntimeError("URL port is empty");

		m_port.assign(port, addr_end - port);
	}

	const char *path = addr_end;
	if (*path == '\0')
		m_path = "/";
	else
		m_path = path;
}

} // namespace
