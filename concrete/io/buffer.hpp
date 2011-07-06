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
#include <concrete/util/noncopyable.hpp>

namespace concrete {

class BufferResource: Noncopyable {
public:
	BufferResource();
	explicit BufferResource(size_t size);
	BufferResource(void *data, size_t size);
	~BufferResource() throw ();

	void reset();
	void reset(size_t size);
	void reset(void *data, size_t size);

	char *data() throw ();
	const char *data() const throw ();
	size_t size() const throw ();
	size_t remaining() const throw ();

	bool read(FileResource &source);
	bool write(FileResource &target);

private:
	char *m_data;
	size_t m_size;
	size_t m_transferred;
};

} // namespace

#endif
