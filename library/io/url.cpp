/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "url.hpp"

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

URL::URL(const char *url)
{
	size_t len = std::strlen(url);
	if (len > MaxLength)
		throw RuntimeError("URL too long");

	const char *url_end = url + len;

	if (std::strncmp(url, "http://", std::strlen("http://")) != 0)
		throw RuntimeError("URL scheme not supported");

	const char *addr = url + std::strlen("http://");
	const char *addr_end = std::strchr(addr, '/');
	if (addr_end == NULL)
		addr_end = url_end;

	const char *host = addr;
	const char *host_end = std::strchr(addr, ':');
	if (host_end == NULL || host_end > addr_end)
		host_end = addr_end;

	if (host == host_end)
		throw RuntimeError("URL host is empty");

	m_host.assign(host, host_end - host);

	if (host_end == addr_end) {
		m_port = "80";
	} else {
		const char *port = host_end + 1;
		if (port == addr_end)
			throw RuntimeError("URL port is empty");

		m_port.assign(port, addr_end - port);
	}

	const char *path = addr_end;
	if (*path == '\0')
		m_path = "/";
	else
		m_path = path;
}

URLOpener *URLOpener::New(const StringObject &url, Buffer *response, Buffer *request)
{
	return new LibraryURLOpener(url, response, request);
}

LibraryURLOpener::LibraryURLOpener(const StringObject &url, Buffer *response, Buffer *request):
	m_url(url.c_str()),
	m_state(Resolve),
	m_request_content(request),
	m_request_length(request ? request->consumable_size() : 0),
	m_request_sent(0),
	m_response_buffer(response),
	m_response_status(0),
	m_response_length(-1)
{
}

bool LibraryURLOpener::content_consumable()
{
	if (m_state != ReceivingContent)
		return false;

	assert(m_response_buffer);

	return m_response_buffer->consumable_size() > 0;
}

void LibraryURLOpener::suspend_until(State objective)
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

// LibraryURLOpener::resolve() is implemented elsewhere

LibraryURLOpener::State LibraryURLOpener::resolving()
{
	assert(m_addrinfo);

	auto addrinfo = m_addrinfo->resolved();
	if (addrinfo)
		return connect(addrinfo);

	m_addrinfo->suspend_until_resolved();
	return Resolving;
}

LibraryURLOpener::State LibraryURLOpener::connect(const struct addrinfo *addrinfo)
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

LibraryURLOpener::State LibraryURLOpener::connecting()
{
	assert(m_socket);

	if (m_socket->connected())
		return Connected;

	m_socket->suspend_until_connected();
	return Connecting;
}

LibraryURLOpener::State LibraryURLOpener::send_headers()
{
	std::string s;

	if (m_request_content == NULL) {
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

LibraryURLOpener::State LibraryURLOpener::sending_headers()
{
	if (m_socket->write(m_request_headers))
		throw RuntimeError("EOF while sending headers");

	if (m_request_headers.consumable_size() == 0)
		return SentHeaders;

	m_socket->suspend_until_writable();
	return SendingHeaders;
}

LibraryURLOpener::State LibraryURLOpener::send_content()
{
	if (m_request_length == 0)
		return SentContent;
	else
		return sending_content();
}

LibraryURLOpener::State LibraryURLOpener::sending_content()
{
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

LibraryURLOpener::State LibraryURLOpener::receive_headers()
{
	return receiving_headers();
}

LibraryURLOpener::State LibraryURLOpener::receiving_headers()
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

LibraryURLOpener::State LibraryURLOpener::receive_content()
{
	if (m_response_length == 0)
		return received_content();

	return receiving_content();
}

LibraryURLOpener::State LibraryURLOpener::receiving_content()
{
	assert(m_response_buffer);

	size_t size = m_response_buffer->production_space();
	assert(size > 0);

	if (m_response_length > 0) {
		long remaining = m_response_length - m_response_buffer->consumable_size();

		if (remaining <= 0)
			return received_content();

		if (size_t(remaining) < size)
			size = size_t(remaining);
	}

	if (m_socket->read(*m_response_buffer, size)) {
		if (m_response_length > 0) {
			if (m_response_buffer->consumable_size() < size_t(m_response_length))
				throw RuntimeError("EOF while receiving content");
		} else {
			m_response_length = m_response_buffer->consumable_size();
		}

		return received_content();
	}

	if (m_response_length >= 0 && m_response_buffer->consumable_size() == size_t(m_response_length))
		return received_content();

	m_socket->suspend_until_readable();
	return ReceivingContent;
}

LibraryURLOpener::State LibraryURLOpener::received_content()
{
	m_socket.reset();

	if (m_response_length < 0)
		m_response_length = m_response_buffer->consumable_size();

	return ReceivedContent;
}

bool LibraryURLOpener::parse_headers()
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

void LibraryURLOpener::parse_header(const char *line, size_t length)
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

	if (match_header(line, length, "transfer-encoding: chunked"))
		throw RuntimeError("Chunked transfer-encoding not supported");
}

} // namespace
