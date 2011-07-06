/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIB_IO_ANL_HPP
#define LIB_IO_ANL_HPP

#include <csignal>

#include <netdb.h>

#include <concrete/io/file.hpp>
#include <concrete/io/resolve.hpp>

namespace concrete {

class ANL: public Resolve {
	friend class Resolve;

public:
	ANL(const std::string &node, const std::string &service);

private:
	struct Pipe {
		Pipe(int ret, int fd[2]);

		File read;
		File write;
	};

	struct AddrInfo {
		AddrInfo(const std::string &node, const std::string &service);
		~AddrInfo() throw ();

		static void callback(sigval_t sigval) throw ();

		const std::string node;
		const std::string service;
		int pipe_buf[2];
		Pipe pipe;
		struct gaicb cb;
	};

	AddrInfo m_addrinfo;
};

} // namespace

#endif
