/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_IO_HTTP_IMPL_HPP
#define LIBRARY_IO_HTTP_IMPL_HPP

#include <memory>
#include <string>

#include <concrete/io/http.hpp>

#include <library/io/addrinfo.hpp>
#include <library/io/socket.hpp>
#include <library/io/url.hpp>

struct addrinfo;

namespace concrete {

class HTTPTransactionImpl: public HTTPTransaction {
	friend class HTTPTransaction;

public:
	HTTPTransactionImpl(HTTP::Method method, const StringObject &url, size_t request_length);
	virtual ~HTTPTransactionImpl() throw () {}

private:
	enum State {
		Resolve,
		Resolving,
		Resolved,
		Connecting,
		Connected,
		SendingHeaders,
		SentHeaders,
		SendingContent,
		SentContent,
		ReceivingHeaders,
		ReceivedHeaders,
		ReceivingContent,
		ReceivedContent,
	};

	void suspend_until(State objective);

	State init_resolve();
	State cont_resolve();
	State init_connect(const struct addrinfo *addrinfo);
	State cont_connect();
	State init_send_headers();
	State cont_send_headers();
	State init_send_content();
	State cont_send_content();
	State init_receive_headers();
	State cont_receive_headers();
	State init_receive_content();
	State cont_receive_content();
	State done_receive_content();

	bool parse_headers();
	void parse_header(const char *line, size_t length);

	const HTTP::Method        m_method;
	const URL                 m_url;

	State                     m_state;
	std::unique_ptr<AddrInfo> m_addrinfo;
	std::unique_ptr<Socket>   m_socket;

	size_t                    m_request_length;
	Buffer                    m_request_headers;
	Buffer                   *m_request_buffer;
	size_t                    m_request_sent;

	Buffer                   *m_response_buffer;
	unsigned int              m_response_status;
	long                      m_response_length;
	size_t                    m_response_received;
};

} // namespace

#endif
