/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_IO_SOCKET_HPP
#define CONCRETE_IO_SOCKET_HPP

#include <sys/socket.h>

#include <concrete/io/file.hpp>

namespace concrete {

class Socket: public File {
public:
	Socket(int domain, int type, int protocol = 0);

	void wait_connection(const struct sockaddr *addr, socklen_t addrlen);
	void wait_connection();
	bool connected();

private:
	bool m_connected;
};

} // namespace

#endif
