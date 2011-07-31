/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "http.hpp"

#include <cctype>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>

#include <boost/format.hpp>

#include <concrete/exception.hpp>
#include <concrete/util/assert.hpp>

namespace concrete {

SysHTTPTransaction::SysHTTPTransaction(HTTP::Method method,
                                       const StringObject &url,
                                       Buffer *request_content):
	m_method(method),
	m_url(url.c_str()),
	m_state(Resolve),
	m_request_content(request_content),
	m_request_length(request_content ? request_content->consumable_size() : 0),
	m_request_sent(0),
	m_response_buffer(NULL),
	m_response_status(0),
	m_response_length(-1),
	m_response_received(0)
{
}

void SysHTTPTransaction::suspend_until(State objective)
{
	if (objective <= m_state) {
		assert(false);
		return;
	}

	switch (m_state) {
	case Resolve:          m_state = resolve();           break;
	case Resolving:        m_state = resolving();         break;
	case Resolved:         assert(false);                 break;
	case Connecting:       m_state = connecting();        break;
	case Connected:        m_state = send_headers();      break;
	case SendingHeaders:   m_state = sending_headers();   break;
	case SentHeaders:      m_state = send_content();      break;
	case SendingContent:   m_state = sending_content();   break;
	case SentContent:      m_state = receive_headers();   break;
	case ReceivingHeaders: m_state = receiving_headers(); break;
	case ReceivedHeaders:  m_state = receive_content();   break;
	case ReceivingContent: m_state = receiving_content(); break;
	case ReceivedContent:  assert(false);                 break;
	}
}

SysHTTPTransaction::State SysHTTPTransaction::resolve()
{
	m_addrinfo.reset(new AddrInfo(m_url.host(), m_url.port()));

	return resolving();
}

SysHTTPTransaction::State SysHTTPTransaction::resolving()
{
	assert(m_addrinfo);

	auto addrinfo = m_addrinfo->resolved();
	if (addrinfo)
		return connect(addrinfo);

	m_addrinfo->suspend_until_resolved();
	return Resolving;
}

SysHTTPTransaction::State SysHTTPTransaction::connect(const struct addrinfo *addrinfo)
{
	int family;
	socklen_t addrlen = 0;
	const struct sockaddr *addr = NULL;

	for (auto i = addrinfo; i; i = i->ai_next)
		if (i->ai_socktype == SOCK_STREAM) {
			family = i->ai_family;
			addrlen = i->ai_addrlen;
			addr = i->ai_addr;
			break;
		}

	if (addr == NULL)
		throw ResourceError();

	m_socket.reset(new Socket(family, SOCK_STREAM));
	m_socket->connect(addr, addrlen);

	m_addrinfo.reset();

	return connecting();
}

SysHTTPTransaction::State SysHTTPTransaction::connecting()
{
	assert(m_socket);

	if (m_socket->connected())
		return Connected;

	m_socket->suspend_until_connected();
	return Connecting;
}

SysHTTPTransaction::State SysHTTPTransaction::send_headers()
{
	std::string s;

	if (m_method == HTTP::GET) {
		s = (boost::format("GET %1% HTTP/1.1\r\n"
		                   "Host: %2%\r\n"
		                   "\r\n")
		     % m_url.path()
		     % m_url.host()).str();
	} else {
		s = (boost::format("POST %1% HTTP/1.1\r\n"
		                   "Host: %2%\r\n"
		                   "Content-Length: %3%\r\n"
		                   "\r\n")
		     % m_url.path()
		     % m_url.host()
		     % m_request_length).str();
	}

	m_request_headers.reset(s.length());
	m_request_headers.produce(s.c_str(), s.length());

	return sending_headers();
}

SysHTTPTransaction::State SysHTTPTransaction::sending_headers()
{
	if (m_socket->write(m_request_headers))
		throw RuntimeError("EOF while sending headers");

	if (m_request_headers.consumable_size() == 0)
		return SentHeaders;

	m_socket->suspend_until_writable();
	return SendingHeaders;
}

SysHTTPTransaction::State SysHTTPTransaction::send_content()
{
	if (m_method == HTTP::GET || m_request_length == 0)
		return SentContent;
	else
		return sending_content();
}

SysHTTPTransaction::State SysHTTPTransaction::sending_content()
{
	assert(m_method == HTTP::POST);
	assert(m_request_length > 0);
	assert(m_request_content);

	size_t size = m_request_content->consumable_size();
	assert(size > 0);

	size_t remaining = m_request_length - m_request_sent;
	assert(remaining > 0);
	if (remaining < size)
		size = remaining;

	size_t mark = m_request_content->consumable_size();

	if (m_socket->write(*m_request_content, size))
		throw RuntimeError("EOF while sending content");

	m_request_sent += mark - m_request_content->consumable_size();
	if (m_request_sent == m_request_length)
		return SentContent;

	m_socket->suspend_until_writable();
	return SendingContent;
}

SysHTTPTransaction::State SysHTTPTransaction::receive_headers()
{
	return receiving_headers();
}

SysHTTPTransaction::State SysHTTPTransaction::receiving_headers()
{
	assert(m_response_buffer);

	bool eof = m_socket->read(*m_response_buffer);

	if (parse_headers()) {
		if (m_response_buffer->consumable_size() == 0) {
			return ReceivedHeaders;
		} else {
			m_response_buffer->shift();
			return ReceivingContent;
		}
	}

	if (eof)
		throw RuntimeError("EOF while receiving headers");

	m_response_buffer->shift();

	m_socket->suspend_until_readable();
	return ReceivingHeaders;
}

SysHTTPTransaction::State SysHTTPTransaction::receive_content()
{
	if (m_response_length == 0)
		return received_content();

	return receiving_content();
}

SysHTTPTransaction::State SysHTTPTransaction::receiving_content()
{
	assert(m_response_buffer);

	size_t size = m_response_buffer->production_space();
	assert(size > 0);

	if (m_response_length > 0) {
		size_t remaining = m_response_length - m_response_received;
		assert(remaining > 0);
		if (remaining < size)
			size = remaining;
	}

	size_t mark = m_response_buffer->consumable_size();

	if (m_socket->read(*m_response_buffer, size)) {
		if (m_response_length > 0) {
			if (m_response_received < size_t(m_response_length))
				throw RuntimeError("EOF while receiving content");
		} else {
			m_response_length = m_response_received;
		}

		return received_content();
	}

	m_response_received += m_response_buffer->consumable_size() - mark;
	if (m_response_length > 0 && m_response_received == size_t(m_response_length))
		return received_content();

	m_socket->suspend_until_readable();
	return ReceivingContent;
}

SysHTTPTransaction::State SysHTTPTransaction::received_content()
{
	m_socket.reset();

	return ReceivedContent;
}

bool SysHTTPTransaction::parse_headers()
{
next:
	while (true) {
		const char *data = m_response_buffer->consumable_data();
		size_t size = m_response_buffer->consumable_size();

		for (size_t i = 0; i < size; i++)
			if (data[i] == '\n') {
				m_response_buffer->consumed_length(i + 1);

				size_t len = i;

				if (len > 0 && data[len - 1] == '\r')
					len--;

				if (len == 0)
					return true;

				parse_header(data, len);
				goto next;
			}

		return false;
	}
}

static bool match_header(const char *line, size_t linelen, const char *prefix) throw ()
{
	size_t prefixlen = std::strlen(prefix);
	return linelen >= prefixlen && strncasecmp(line, prefix, prefixlen) == 0;
}

static const char *header_value(const char *line) throw ()
{
	const char *ptr = std::strchr(line, ':');
	assert(ptr);

	do {
		ptr++;
	} while (*ptr != '\r' && *ptr != '\n' && std::isspace(*ptr));

	return ptr;
}

void SysHTTPTransaction::parse_header(const char *line, size_t length)
{
	if (m_response_status == 0) {
		if (std::strncmp(line, "HTTP/1.0 ", 9) != 0 && std::strncmp(line, "HTTP/1.1 ", 9) != 0)
			throw RuntimeError("Unsupported protocol in response");

		char *end;
		size_t code = std::strtoul(line + 9, &end, 10);
		if (!std::isspace(*end) || code == 0)
			throw RuntimeError("Invalid response status");

		// skip provisional 1xx responses
		if (code >= 100 && code < 200)
			return;

		m_response_status = code;
		return;
	}

	if (match_header(line, length, "content-length:")) {
		char *end;
		size_t value = std::strtoul(header_value(line), &end, 10);
		if (!std::isspace(*end))
			throw RuntimeError("Invalid Content-Length response header");

		if (m_response_length >= 0 && value != size_t(m_response_length))
			throw RuntimeError("Duplicate Content-Length response header");

		m_response_length = value;
		return;
	}
}

void HTTPTransaction::reset_response_buffer(Buffer *buffer)
{
	static_cast<SysHTTPTransaction *> (this)->m_response_buffer = buffer;
}

HTTP::Status HTTPTransaction::response_status() const
{
	auto impl = static_cast<const SysHTTPTransaction *> (this);

	assert(impl->m_state >= SysHTTPTransaction::ReceivedHeaders);

	auto status = impl->m_response_status;
	assert(status > 0);

	return HTTP::Status(status);
}

long HTTPTransaction::response_length() const
{
	auto impl = static_cast<const SysHTTPTransaction *> (this);

	assert(impl->m_state >= SysHTTPTransaction::ReceivedHeaders);

	return impl->m_response_length;
}

bool HTTPTransaction::headers_received()
{
	return static_cast<SysHTTPTransaction *> (this)->m_state >= SysHTTPTransaction::ReceivedHeaders;
}

bool HTTPTransaction::content_consumable()
{
	auto impl = static_cast<SysHTTPTransaction *> (this);

	if (impl->m_state != SysHTTPTransaction::ReceivingContent)
		return false;

	assert(impl->m_response_buffer);

	return impl->m_response_buffer->consumable_size() > 0;
}

bool HTTPTransaction::content_received()
{
	return static_cast<SysHTTPTransaction *> (this)->m_state >= SysHTTPTransaction::ReceivedContent;
}

void HTTPTransaction::suspend_until_headers_received()
{
	static_cast<SysHTTPTransaction *> (this)->suspend_until(SysHTTPTransaction::ReceivedHeaders);
}

void HTTPTransaction::suspend_until_content_consumable()
{
	suspend_until_content_received();
}

void HTTPTransaction::suspend_until_content_received()
{
	static_cast<SysHTTPTransaction *> (this)->suspend_until(SysHTTPTransaction::ReceivedContent);
}

HTTPTransaction *ResourceCreate<HTTPTransaction>::New(HTTP::Method method,
                                                      const StringObject &url,
                                                      Buffer *request_content)
{
	return new SysHTTPTransaction(method, url, request_content);
}

} // namespace
