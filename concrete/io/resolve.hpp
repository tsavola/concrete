/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_IO_RESOLVE_HPP
#define CONCRETE_IO_RESOLVE_HPP

#include <cassert>
#include <csignal>
#include <cstdint>
#include <string>

#include <netdb.h>

#include <concrete/io/file.hpp>
#include <concrete/resource.hpp>

namespace concrete {

class ResolveResource: public Resource {
	struct Pipe {
		FileResource read;
		FileResource write;

		Pipe(int ret, int fd[2]);
	};

	struct AddrInfo {
		const std::string node;
		const std::string service;
		int pipe_buf[2];
		Pipe pipe;
		struct gaicb cb;

		AddrInfo(const std::string &node, const std::string &service);
		~AddrInfo() throw ();

		static void callback(sigval_t sigval) throw ();
	};

public:
	ResolveResource(const std::string &node, const std::string &service);

	void wait_addrinfo();
	struct addrinfo *addrinfo();

private:
	AddrInfo m_addrinfo;
};

} // namespace

#endif
