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

#include <concrete/context.hpp>
#include <concrete/event.hpp>

namespace concrete {

File::File(int fd):
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

File::~File() throw ()
{
	close(m_fd);
}

int File::fd() const throw ()
{
	return m_fd;
}

void File::suspend_until(unsigned int conditions)
{
	Context::Active().suspend_until(fd(), conditions);
}

void File::suspend_until_readable()
{
	suspend_until(EventSourceReadable);
}

void File::suspend_until_writable()
{
	suspend_until(EventSourceWritable);
}

ssize_t File::read(void *buf, size_t bufsize)
{
	auto len = ::read(fd(), buf, bufsize);

	if (len < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
		throw ResourceError();

	return len;
}

ssize_t File::write(const void *buf, size_t bufsize)
{
	auto len = ::write(fd(), buf, bufsize);

	if (len < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
		throw ResourceError();

	return len;
}

} // namespace
