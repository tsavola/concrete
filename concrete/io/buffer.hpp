/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_IO_BUFFER_HPP
#define CONCRETE_IO_BUFFER_HPP

#include <cstddef>

#include <concrete/io/file.hpp>
#include <concrete/resource.hpp>

namespace concrete {

class BufferResource: public Resource {
public:
	explicit BufferResource(size_t size);
	BufferResource(void *data, size_t size);
	virtual ~BufferResource() throw ();

	char *data() throw ()
	{
		return m_data;
	}

	const char *data() const throw ()
	{
		return m_data;
	}

	size_t size() const throw ()
	{
		return m_size;
	}

	size_t remaining() const throw ()
	{
		return m_size - m_transferred;
	}

	bool read(FileResource &source);
	bool write(FileResource &target);

private:
	char *const m_data;
	const size_t m_size;
	size_t m_transferred;
};

} // namespace

#endif
