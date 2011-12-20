/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "addrinfo.hpp"

#include <cstddef>

#include <concrete/context.hpp>
#include <concrete/resource.hpp>

#include <library/libevent/event.hpp>

namespace concrete {

EvdnsAddrInfo::EvdnsAddrInfo(const std::string &node, const std::string &service):
	m_node(node),
	m_service(service),
	m_base(static_cast<LibeventLoop &> (Context::Active().event_loop()).new_evdns_base()),
	m_request(NULL),
	m_result(-1),
	m_addrinfo(NULL)
{
	m_request = evdns_getaddrinfo(m_base, m_node.c_str(), m_service.c_str(), NULL, callback, reinterpret_cast<void *> (this));
	if (m_request == NULL)
		throw ResourceError();
}

EvdnsAddrInfo::~EvdnsAddrInfo() throw ()
{
	evdns_base_free(m_base, true);
}

struct addrinfo *EvdnsAddrInfo::resolved()
{
	char buf;

	if (m_pipe.read.read(&buf, 1) < 0)
		return NULL;

	if (m_request == NULL) {
		if (m_result != 0)
			throw ResourceError();

		return m_addrinfo;
	}

	return NULL;
}

void EvdnsAddrInfo::suspend_until_resolved()
{
	m_pipe.read.suspend_until_readable();
}

void EvdnsAddrInfo::callback(int result, struct addrinfo *addrinfo, void *arg)
{
	auto self = reinterpret_cast<EvdnsAddrInfo *> (arg);
	self->m_request = NULL;
	self->m_result = result;
	self->m_addrinfo = addrinfo;
	self->m_pipe.write_byte_blocking();
}

} // namespace
