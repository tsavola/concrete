/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "socket.hpp"

#include <cerrno>

#include <concrete/context.hpp>
#include <concrete/resource.hpp>
#include <concrete/util/assert.hpp>

#include <library/io/error.hpp>

namespace concrete {

Socket::Socket(int domain, int type, int protocol):
	File(socket(domain, type, protocol)),
	m_connected(false)
{
}

void Socket::connect(const struct sockaddr *addr, socklen_t addrlen)
{
	assert(!m_connected);

	if (::connect(fd(), addr, addrlen) < 0) {
		if (errno != EINPROGRESS)
			throw IOResourceError();
	} else {
		m_connected = true;
	}
}

bool Socket::connected()
{
	if (m_connected)
		return true;

	int error;
	socklen_t len = sizeof (error);

	if (getsockopt(fd(), SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
		if (errno == EINPROGRESS)
			return false;

		throw IOResourceError();
	}

	if (error == 0) {
		m_connected = true;
		return true;
	}

	if (error == EINPROGRESS)
		return false;

	throw IOResourceError(error);
}

void Socket::suspend_until_connected()
{
	suspend_until_writable();
}

} // namespace
