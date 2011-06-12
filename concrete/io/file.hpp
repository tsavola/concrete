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

#include <sys/types.h>

#include <concrete/resource.hpp>

namespace concrete {

class FileResource: public Resource {
public:
	static int NonthrowingNonblock(int fd) throw ();

	explicit FileResource(int fd);
	virtual ~FileResource() throw ();

	int fd() const throw ();

	void wait_readability();
	ssize_t read(void *buf, size_t bufsize);

	void wait_writability();
	ssize_t write(const void *buf, size_t bufsize);

private:
	const int m_fd;
};

} // namespace

#endif
