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

class Buffer: public Resource {
public:
	Buffer();
	explicit Buffer(size_t size);
	Buffer(void *data, size_t size);
	~Buffer() throw ();

	void reset();
	void reset(size_t size);
	void reset(void *data, size_t size);

	char *data() throw ();
	const char *data() const throw ();
	size_t size() const throw ();
	size_t remaining() const throw ();

	bool read(File &source);
	bool write(File &target);

private:
	char   *m_data;
	size_t  m_size;
	size_t  m_transferred;
};

} // namespace

#endif
