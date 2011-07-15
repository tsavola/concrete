/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_IO_ADDRINFO_HPP
#define LIBRARY_IO_ADDRINFO_HPP

#include <string>

#include <netdb.h>

#include <concrete/util/noncopyable.hpp>

#include <library/io/file.hpp>

namespace concrete {

class AddrInfo: Noncopyable {
public:
	AddrInfo(const std::string &node, const std::string &service);
	virtual ~AddrInfo() throw ();

	struct addrinfo *resolved();
	void suspend_until_resolved();

private:
	struct Pipe {
		Pipe(int ret);

		File read;
		File write;
		int fd[2];
	};

	Pipe m_pipe;
	struct gaicb m_callback;
};

} // namespace

#endif
