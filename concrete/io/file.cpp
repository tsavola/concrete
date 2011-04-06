/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "file.hpp"

#include <cerrno>

#include <fcntl.h>
#include <unistd.h>

#include <event.h>

#include <concrete/context.hpp>
#include <concrete/resource.hpp>

namespace concrete {

FileResource::FileResource(int fd):
	m_fd(fd)
{
	if (m_fd < 0)
		throw ResourceError();

	int flags = fcntl(m_fd, F_GETFL);
	if (flags < 0) {
		close(m_fd);
		throw ResourceError();
	}
	if (fcntl(m_fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		close(m_fd);
		throw ResourceError();
	}
}

FileResource::~FileResource() throw ()
{
	close(m_fd);
}

void FileResource::wait_readability()
{
	Context::WaitEvent(fd(), EV_READ);
}

ssize_t FileResource::read(void *buf, size_t bufsize)
{
	auto len = ::read(fd(), buf, bufsize);

	if (len < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
		throw ResourceError();

	return len;
}

void FileResource::wait_writability()
{
	Context::WaitEvent(fd(), EV_WRITE);
}

ssize_t FileResource::write(const void *buf, size_t bufsize)
{
	auto len = ::write(fd(), buf, bufsize);

	if (len < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
		throw ResourceError();

	return len;
}

} // namespace
