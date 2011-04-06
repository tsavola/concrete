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

BufferResource::BufferResource(size_t size):
	m_data(new char[size]),
	m_size(size),
	m_transferred(0)
{
}

BufferResource::BufferResource(void *data, size_t size):
	m_data(new char[size]),
	m_size(size),
	m_transferred(0)
{
	std::memcpy(m_data, data, size);
}

BufferResource::~BufferResource() throw ()
{
	delete[] m_data;
}

bool BufferResource::read(FileResource &file)
{
	auto len = file.read(m_data + m_transferred, m_size - m_transferred);

	if (len > 0)
		m_transferred += len;

	return len != 0;
}

bool BufferResource::write(FileResource &file)
{
	auto len = file.write(m_data + m_transferred, m_size - m_transferred);

	if (len > 0)
		m_transferred += len;

	return len != 0;
}

} // namespace
