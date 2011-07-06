/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "resolve.hpp"

#include <cerrno>
#include <cstring>

#include <unistd.h>

#include <concrete/context.hpp>

namespace concrete {

Resolve::Pipe::Pipe(int ret, int fd[2]):
	read(ret >= 0 ? fd[0] : -1),
	write(ret >= 0 ? fd[1] : -1)
{
}

Resolve::AddrInfo::AddrInfo(const std::string &node_, const std::string &service_):
	node(node_),
	service(service_),
	pipe(::pipe(pipe_buf), pipe_buf)
{
	std::memset(&cb, 0, sizeof (cb));
	cb.ar_name = node.c_str();
	cb.ar_service = service.c_str();

	struct gaicb *list[] = { &cb };

	struct sigevent ev;
	std::memset(&ev, 0, sizeof (ev));
	ev.sigev_value.sival_int = pipe.write.fd();
	ev.sigev_notify = SIGEV_THREAD;
	ev.sigev_notify_function = callback;

	if (getaddrinfo_a(GAI_NOWAIT, list, 1, &ev) != 0)
		throw ResourceError();
}

Resolve::AddrInfo::~AddrInfo() throw ()
{
	gai_cancel(&cb);

	if (cb.ar_result)
		freeaddrinfo(cb.ar_result);
}

void Resolve::AddrInfo::callback(sigval_t sigval) throw ()
{
	int fd = sigval.sival_int;
	char buf = 0;

	while (write(fd, &buf, 1) < 0 && (errno == EINTR || errno == EAGAIN))
		;
}

Resolve::Resolve(const std::string &node, const std::string &service):
	m_addrinfo(node, service)
{
}

void Resolve::suspend_until_resolved()
{
	m_addrinfo.pipe.read.suspend_until_readable();
}

struct addrinfo *Resolve::addrinfo()
{
	char buf;

	if (m_addrinfo.pipe.read.read(&buf, 1) < 0)
		return NULL;

	if (gai_error(&m_addrinfo.cb) != 0)
		throw ResourceError();

	return m_addrinfo.cb.ar_result;
}

} // namespace
