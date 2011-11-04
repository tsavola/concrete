/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_IO_LIBEVENT_ADDRINFO_HPP
#define LIBRARY_IO_LIBEVENT_ADDRINFO_HPP

#include <string>

#include <evdns.h>

#include <library/io/addrinfo.hpp>
#include <library/io/file.hpp>

namespace concrete {

class EvdnsAddrInfo: public AddrInfo {
public:
	EvdnsAddrInfo(const std::string &node, const std::string &service);
	virtual ~EvdnsAddrInfo() throw ();

	virtual struct addrinfo *resolved();
	virtual void suspend_until_resolved();

private:
	static void callback(int result, struct addrinfo *addrinfo, void *arg);

	const std::string                 m_node;
	const std::string                 m_service;
	Pipe                              m_pipe;
	struct evdns_base                *m_base;
	struct evdns_getaddrinfo_request *m_request;
	int                               m_result;
	struct addrinfo                  *m_addrinfo;
};

} // namespace

#endif
