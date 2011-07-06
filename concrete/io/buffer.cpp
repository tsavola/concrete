/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "buffer.hpp"

#include <cstring>

namespace concrete {

Buffer::Buffer():
	m_data(NULL),
	m_size(0),
	m_transferred(0)
{
}

Buffer::Buffer(size_t size):
	m_data(new char[size]),
	m_size(size),
	m_transferred(0)
{
}

Buffer::Buffer(void *data, size_t size):
	m_data(new char[size]),
	m_size(size),
	m_transferred(0)
{
	std::memcpy(m_data, data, size);
}

Buffer::~Buffer() throw ()
{
	delete[] m_data;
}

void Buffer::reset()
{
	if (m_data)
		delete[] m_data;

	m_data = NULL;
	m_size = 0;
	m_transferred = 0;
}

void Buffer::reset(size_t size)
{
	reset();

	m_data = new char[size];
	m_size = size;
}

void Buffer::reset(void *data, size_t size)
{
	reset(size);

	std::memcpy(m_data, data, size);
}

char *Buffer::data() throw ()
{
	return m_data;
}

const char *Buffer::data() const throw ()
{
	return m_data;
}

size_t Buffer::size() const throw ()
{
	return m_size;
}

size_t Buffer::remaining() const throw ()
{
	return m_size - m_transferred;
}

bool Buffer::read(File &file)
{
	auto len = file.read(m_data + m_transferred, m_size - m_transferred);

	if (len > 0)
		m_transferred += len;

	return len != 0;
}

bool Buffer::write(File &file)
{
	auto len = file.write(m_data + m_transferred, m_size - m_transferred);

	if (len > 0)
		m_transferred += len;

	return len != 0;
}

} // namespace
