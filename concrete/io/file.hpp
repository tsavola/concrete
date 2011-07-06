/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_IO_FILE_HPP
#define CONCRETE_IO_FILE_HPP

#include <cstddef>

#include <concrete/resource.hpp>

namespace concrete {

class File: public Resource {
public:
	explicit File(int fd);
	virtual ~File() throw ();

	int fd() const throw ();

	void suspend_until(unsigned int conditions);
	void suspend_until_readable();
	void suspend_until_writable();

	ssize_t read(void *buf, size_t bufsize);
	ssize_t write(const void *buf, size_t bufsize);

private:
	const int m_fd;
};

} // namespace

#endif
