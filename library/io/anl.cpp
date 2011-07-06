/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "anl.hpp"

#include <cerrno>
#include <cstring>

#include <unistd.h>

#include <concrete/context.hpp>

namespace concrete {

ANL::Pipe::Pipe(int ret, int fd[2]):
	read(ret >= 0 ? fd[0] : -1),
	write(ret >= 0 ? fd[1] : -1)
{
}

ANL::AddrInfo::AddrInfo(const std::string &node_, const std::string &service_):
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

ANL::AddrInfo::~AddrInfo() throw ()
{
	gai_cancel(&cb);

	if (cb.ar_result)
		freeaddrinfo(cb.ar_result);
}

void ANL::AddrInfo::callback(sigval_t sigval) throw ()
{
	int fd = sigval.sival_int;
	char buf = 0;

	while (write(fd, &buf, 1) < 0 && (errno == EINTR || errno == EAGAIN))
		;
}

ANL::ANL(const std::string &node, const std::string &service):
	m_addrinfo(node, service)
{
}

void Resolve::suspend_until_resolved()
{
	auto &addrinfo = static_cast<ANL *> (this)->m_addrinfo;

	addrinfo.pipe.read.suspend_until_readable();
}

struct addrinfo *Resolve::addrinfo()
{
	auto &addrinfo = static_cast<ANL *> (this)->m_addrinfo;
	char buf;

	if (addrinfo.pipe.read.read(&buf, 1) < 0)
		return NULL;

	if (gai_error(&addrinfo.cb) != 0)
		throw ResourceError();

	return addrinfo.cb.ar_result;
}

Resolve *ResourceCreate<Resolve>::New(const std::string &node, const std::string &service)
{
	return new ANL(node, service);
}

} // namespace
