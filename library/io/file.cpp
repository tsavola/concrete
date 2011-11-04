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
#include <concrete/util/assert.hpp>

#include <library/event.hpp>
#include <library/io/error.hpp>

namespace concrete {

File::File(int fd): m_fd(-1)
{
	init(fd);
}

File::~File() throw ()
{
	if (m_fd >= 0)
		close(m_fd);
}

void File::init(int fd)
{
	assert(m_fd < 0);

	if (fd < 0)
		throw IOResourceError();

	int flags = fcntl(fd, F_GETFL);
	if (flags < 0) {
		int error = errno;
		close(fd);
		throw IOResourceError(error);
	}
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
		int error = errno;
		close(fd);
		throw IOResourceError(error);
	}

	m_fd = fd;
}

int File::fd() const throw ()
{
	assert(m_fd >= 0);

	return m_fd;
}

bool File::read(Buffer &buffer)
{
	assert(m_fd >= 0);

	return read(buffer, buffer.production_space());
}

bool File::read(Buffer &buffer, size_t size)
{
	assert(m_fd >= 0);
	assert(size <= buffer.production_space());

	if (size == 0)
		return true;

	ssize_t len = read(buffer.production_ptr(), size);
	if (len > 0)
		buffer.produced_length(len);

	return len == 0;
}

ssize_t File::read(char *data, size_t size)
{
	assert(m_fd >= 0);

	auto len = ::read(fd(), data, size);

	if (len < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
		throw IOResourceError();

	return len;
}

bool File::write(Buffer &buffer)
{
	assert(m_fd >= 0);

	return write(buffer, buffer.consumable_size());
}

bool File::write(Buffer &buffer, size_t size)
{
	assert(m_fd >= 0);
	assert(size <= buffer.consumable_size());

	if (size == 0)
		return true;

	ssize_t len = write(buffer.consumable_data(), size);
	if (len > 0)
		buffer.consumed_length(len);

	return len == 0;
}

ssize_t File::write(const char *data, size_t size)
{
	assert(m_fd >= 0);

	auto len = ::write(fd(), data, size);

	if (len < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)
		throw IOResourceError();

	return len;
}

void File::suspend_until(unsigned int conditions)
{
	assert(m_fd >= 0);

	Context::Active().suspend_until(EventTrigger(fd(), conditions));
}

void File::suspend_until_readable()
{
	suspend_until(EventFileReadable);
}

void File::suspend_until_writable()
{
	suspend_until(EventFileWritable);
}

Pipe::Pipe()
{
	int fd[2];

	if (pipe(fd) < 0)
		throw IOResourceError();

	read.init(fd[0]);
	write.init(fd[1]);
}

void Pipe::write_byte_blocking(uint8_t value)
{
	while (::write(write.fd(), &value, 1) < 0 && (errno == EINTR || errno == EAGAIN))
		;
}

} // namespace
