/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "http-impl.hpp"

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>

#include <boost/format.hpp>

#include <concrete/exception.hpp>

namespace concrete {

HTTPTransactionImpl::HTTPTransactionImpl(HTTP::Method method,
                                         const StringObject &url,
                                         size_t request_length):
	m_method(method),
	m_url(url.c_str()),
	m_state(Resolve),
	m_request_length(request_length),
	m_request_buffer(NULL),
	m_request_sent(0),
	m_response_buffer(NULL),
	m_response_status(0),
	m_response_length(-1),
	m_response_received(0)
{
}

void HTTPTransactionImpl::suspend_until(State objective)
{
	if (objective <= m_state) {
		assert(false);
		return;
	}

	switch (m_state) {
	case Resolve:          m_state = init_resolve();         break;
	case Resolving:        m_state = cont_resolve();         break;
	case Resolved:                   assert(false);          break;
	case Connecting:       m_state = cont_connect();         break;
	case Connected:        m_state = init_send_headers();    break;
	case SendingHeaders:   m_state = cont_send_headers();    break;
	case SentHeaders:      m_state = init_send_content();    break;
	case SendingContent:   m_state = cont_send_content();    break;
	case SentContent:      m_state = init_receive_headers(); break;
	case ReceivingHeaders: m_state = cont_receive_headers(); break;
	case ReceivedHeaders:  m_state = init_receive_content(); break;
	case ReceivingContent: m_state = cont_receive_content(); break;
	case ReceivedContent:            assert(false);          break;
	}
}

HTTPTransactionImpl::State HTTPTransactionImpl::init_resolve()
{
	m_addrinfo.reset(new AddrInfo(m_url.host(), m_url.port()));

	return cont_resolve();
}

HTTPTransactionImpl::State HTTPTransactionImpl::cont_resolve()
{
	assert(m_addrinfo);

	auto addrinfo = m_addrinfo->resolved();
	if (addrinfo)
		return init_connect(addrinfo);

	m_addrinfo->suspend_until_resolved();
	return Resolving;
}

HTTPTransactionImpl::State HTTPTransactionImpl::init_connect(const struct addrinfo *addrinfo)
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

	return cont_connect();
}

HTTPTransactionImpl::State HTTPTransactionImpl::cont_connect()
{
	assert(m_socket);

	if (m_socket->connected())
		return Connected;

	m_socket->suspend_until_connected();
	return Connecting;
}

HTTPTransactionImpl::State HTTPTransactionImpl::init_send_headers()
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

	return cont_send_headers();
}

HTTPTransactionImpl::State HTTPTransactionImpl::cont_send_headers()
{
	if (m_socket->write(m_request_headers))
		throw RuntimeError("EOF while sending headers");

	if (m_request_headers.consumable_size() == 0)
		return SentHeaders;

	m_socket->suspend_until_writable();
	return SendingHeaders;
}

HTTPTransactionImpl::State HTTPTransactionImpl::init_send_content()
{
	if (m_method == HTTP::GET || m_request_length == 0)
		return SentContent;
	else
		return cont_send_content();
}

HTTPTransactionImpl::State HTTPTransactionImpl::cont_send_content()
{
	assert(m_method == HTTP::POST);
	assert(m_request_length > 0);
	assert(m_request_buffer);

	size_t size = m_request_buffer->consumable_size();
	assert(size > 0);

	size_t remaining = m_request_length - m_request_sent;
	assert(remaining > 0);
	if (remaining < size)
		size = remaining;

	size_t mark = m_request_buffer->consumable_size();

	if (m_socket->write(*m_request_buffer, size))
		throw RuntimeError("EOF while sending content");

	m_request_sent += mark - m_request_buffer->consumable_size();
	if (m_request_sent == m_request_length)
		return SentContent;

	m_socket->suspend_until_writable();
	return SendingContent;
}

HTTPTransactionImpl::State HTTPTransactionImpl::init_receive_headers()
{
	return cont_receive_headers();
}

HTTPTransactionImpl::State HTTPTransactionImpl::cont_receive_headers()
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

HTTPTransactionImpl::State HTTPTransactionImpl::init_receive_content()
{
	if (m_response_length == 0)
		return ReceivedContent;

	return cont_receive_content();
}

HTTPTransactionImpl::State HTTPTransactionImpl::cont_receive_content()
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

		return done_receive_content();
	}

	m_response_received += m_response_buffer->consumable_size() - mark;
	if (m_response_length > 0 && m_response_received == size_t(m_response_length))
		return done_receive_content();

	m_socket->suspend_until_readable();
	return ReceivingContent;
}

HTTPTransactionImpl::State HTTPTransactionImpl::done_receive_content()
{
	m_socket.reset();

	return ReceivedContent;
}

bool HTTPTransactionImpl::parse_headers()
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

void HTTPTransactionImpl::parse_header(const char *line, size_t length)
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

void HTTPTransaction::set_request_length(size_t length)
{
	static_cast<HTTPTransactionImpl *> (this)->m_request_length = length;
}

void HTTPTransaction::reset_request_buffer(Buffer *buffer)
{
	static_cast<HTTPTransactionImpl *> (this)->m_request_buffer = buffer;
}

void HTTPTransaction::reset_response_buffer(Buffer *buffer)
{
	static_cast<HTTPTransactionImpl *> (this)->m_response_buffer = buffer;
}

HTTP::Status HTTPTransaction::response_status() const
{
	auto impl = static_cast<const HTTPTransactionImpl *> (this);

	assert(impl->m_state >= HTTPTransactionImpl::ReceivedHeaders);

	auto status = impl->m_response_status;
	assert(status > 0);

	return HTTP::Status(status);
}

long HTTPTransaction::response_length() const
{
	auto impl = static_cast<const HTTPTransactionImpl *> (this);

	assert(impl->m_state >= HTTPTransactionImpl::ReceivedHeaders);

	return impl->m_response_length;
}

bool HTTPTransaction::connected()
{
	return static_cast<HTTPTransactionImpl *> (this)->m_state >= HTTPTransactionImpl::Connected;
}

bool HTTPTransaction::headers_sent()
{
	return static_cast<HTTPTransactionImpl *> (this)->m_state >= HTTPTransactionImpl::SentHeaders;
}

bool HTTPTransaction::content_producable()
{
	auto impl = static_cast<HTTPTransactionImpl *> (this);

	if (impl->m_state != HTTPTransactionImpl::SendingContent)
		return false;

	assert(impl->m_request_buffer);

	return impl->m_request_buffer->production_space() > 0;
}

bool HTTPTransaction::content_sent()
{
	return static_cast<HTTPTransactionImpl *> (this)->m_state >= HTTPTransactionImpl::SentContent;
}

bool HTTPTransaction::headers_received()
{
	return static_cast<HTTPTransactionImpl *> (this)->m_state >= HTTPTransactionImpl::ReceivedHeaders;
}

bool HTTPTransaction::content_consumable()
{
	auto impl = static_cast<HTTPTransactionImpl *> (this);

	if (impl->m_state != HTTPTransactionImpl::ReceivingContent)
		return false;

	assert(impl->m_response_buffer);

	return impl->m_response_buffer->consumable_size() > 0;
}

bool HTTPTransaction::content_received()
{
	return static_cast<HTTPTransactionImpl *> (this)->m_state >= HTTPTransactionImpl::ReceivedContent;
}

void HTTPTransaction::suspend_until_connected()
{
	static_cast<HTTPTransactionImpl *> (this)->suspend_until(HTTPTransactionImpl::Connected);
}

void HTTPTransaction::suspend_until_headers_sent()
{
	static_cast<HTTPTransactionImpl *> (this)->suspend_until(HTTPTransactionImpl::SentHeaders);
}

void HTTPTransaction::suspend_until_content_producable()
{
	suspend_until_content_sent();
}

void HTTPTransaction::suspend_until_content_sent()
{
	static_cast<HTTPTransactionImpl *> (this)->suspend_until(HTTPTransactionImpl::SentContent);
}

void HTTPTransaction::suspend_until_headers_received()
{
	static_cast<HTTPTransactionImpl *> (this)->suspend_until(HTTPTransactionImpl::ReceivedHeaders);
}

void HTTPTransaction::suspend_until_content_consumable()
{
	suspend_until_content_received();
}

void HTTPTransaction::suspend_until_content_received()
{
	static_cast<HTTPTransactionImpl *> (this)->suspend_until(HTTPTransactionImpl::ReceivedContent);
}

HTTPTransaction *ResourceCreate<HTTPTransaction>::New(HTTP::Method method,
                                                      const StringObject &url,
                                                      size_t request_length)
{
	return new HTTPTransactionImpl(method, url, request_length);
}

} // namespace
