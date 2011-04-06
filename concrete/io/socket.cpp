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

#include <event.h>

#include <concrete/context.hpp>
#include <concrete/resource.hpp>

namespace concrete {

SocketResource::SocketResource(int domain, int type, int protocol):
	FileResource(socket(domain, type, protocol)),
	m_connected(false)
{
}

void SocketResource::wait_connection(const struct sockaddr *addr, socklen_t addrlen)
{
	assert(!m_connected);

	if (connect(fd(), addr, addrlen) < 0) {
		if (errno == EINPROGRESS)
			wait_connection();
		else
			throw ResourceError();
	} else {
		m_connected = true;
	}
}

void SocketResource::wait_connection()
{
	Context::WaitEvent(fd(), EV_WRITE);
}

bool SocketResource::connected()
{
	if (m_connected)
		return true;

	int error;
	socklen_t len;

	if (getsockopt(fd(), SOL_SOCKET, SO_ERROR, &error, &len) < 0)
		throw ResourceError();

	if (error == 0) {
		m_connected = true;
		return true;
	}

	if (error == EINPROGRESS)
		return false;

	throw ResourceError();
}

} // namespace
