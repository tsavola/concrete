/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_IO_FILE_HPP
#define LIBRARY_IO_FILE_HPP

#include <cstddef>
#include <cstdint>

#include <concrete/io/buffer.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

class File: Noncopyable {
public:
	File() throw (): m_fd(-1) {}
	explicit File(int fd);
	~File() throw ();

	void init(int fd);

	int fd() const throw ();

	bool read(Buffer &buffer);
	bool read(Buffer &buffer, size_t size);
	ssize_t read(char *data, size_t size);

	bool write(Buffer &buffer);
	bool write(Buffer &buffer, size_t size);
	ssize_t write(const char *data, size_t size);

	void suspend_until(unsigned int conditions);
	void suspend_until_readable();
	void suspend_until_writable();

private:
	int m_fd;
};

class Pipe: Noncopyable {
public:
	Pipe();

	void write_byte_blocking(uint8_t value = 0);

	File read;
	File write;
};

} // namespace

#endif
