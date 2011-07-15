/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "buffer.hpp"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <new>

namespace concrete {

Buffer::Buffer():
	m_data(NULL),
	m_size(0),
	m_produce_offset(0),
	m_consume_offset(0)
{
}

Buffer::Buffer(size_t size):
	m_size(size),
	m_produce_offset(0),
	m_consume_offset(0)
{
	m_data = reinterpret_cast<char *> (std::malloc(size));
	if (m_data == NULL)
		throw std::bad_alloc();
}

Buffer::~Buffer() throw ()
{
	std::free(m_data);
}

void Buffer::reset()
{
	std::free(m_data);

	m_data = NULL;
	m_size = 0;
	m_produce_offset = 0;
	m_consume_offset = 0;
}

void Buffer::reset(size_t size)
{
	char *data = reinterpret_cast<char *> (std::realloc(m_data, size));
	if (data == NULL)
		throw std::bad_alloc();

	m_data = data;
	m_size = size;
	m_produce_offset = 0;
	m_consume_offset = 0;
}

size_t Buffer::production_space() const throw ()
{
	if (m_consume_offset == m_size) {
		assert(m_produce_offset == m_size);

		m_produce_offset = 0;
		m_consume_offset = 0;
	}

	assert(m_produce_offset <= m_size);

	return m_size - m_produce_offset;
}

char *Buffer::production_ptr() const throw ()
{
	return m_data + m_produce_offset;
}

void Buffer::produced_length(size_t length) throw ()
{
	assert(length <= production_space());

	m_produce_offset += length;
}

void Buffer::produce(const void *data, size_t size) throw ()
{
	size_t space = production_space();
	assert(size <= space);
	if (size > space)
		size = space;

	std::memcpy(production_ptr(), data, size);
	produced_length(size);
}

size_t Buffer::consumable_size() const throw ()
{
	assert(m_produce_offset <= m_size);
	assert(m_consume_offset <= m_produce_offset);

	return m_produce_offset - m_consume_offset;
}

const char *Buffer::consumable_data() const throw ()
{
	return m_data + m_consume_offset;
}

void Buffer::consumed_length(size_t length) throw ()
{
	assert(length <= consumable_size());

	m_consume_offset += length;
}

void Buffer::shift() throw ()
{
	std::memmove(m_data, consumable_data(), consumable_size());

	m_produce_offset -= m_consume_offset;
	m_consume_offset = 0;
}

} // namespace
