/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "addrinfo.hpp"

#include <cerrno>
#include <csignal>
#include <cstring>

#include <unistd.h>

namespace concrete {

static void notify(sigval_t sigval) throw ()
{
	int fd = sigval.sival_int;
	char buf = 0;

	while (write(fd, &buf, 1) < 0 && (errno == EINTR || errno == EAGAIN))
		;
}

AsyncAddrInfo::AsyncAddrInfo(const std::string &node, const std::string &service)
{
	std::memset(&m_callback, 0, sizeof (m_callback));

	m_callback.ar_name = node.c_str();
	m_callback.ar_service = service.c_str();

	struct gaicb *list[] = { &m_callback };

	struct sigevent event;
	std::memset(&event, 0, sizeof (event));

	event.sigev_value.sival_int = m_pipe.write.fd();
	event.sigev_notify = SIGEV_THREAD;
	event.sigev_notify_function = notify;

	if (getaddrinfo_a(GAI_NOWAIT, list, 1, &event) != 0)
		throw ResourceError();
}

AsyncAddrInfo::~AsyncAddrInfo() throw ()
{
	gai_cancel(&m_callback);

	if (m_callback.ar_result)
		freeaddrinfo(m_callback.ar_result);
}

struct addrinfo *AsyncAddrInfo::resolved()
{
	char buf;

	if (m_pipe.read.read(&buf, 1) < 0)
		return NULL;

	if (gai_error(&m_callback) != 0)
		throw ResourceError();

	return m_callback.ar_result;
}

void AsyncAddrInfo::suspend_until_resolved()
{
	m_pipe.read.suspend_until_readable();
}

} // namespace
