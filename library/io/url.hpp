/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_IO_URL_HPP
#define LIBRARY_IO_URL_HPP

#include <memory>
#include <string>

#include <concrete/io/url.hpp>

#include <library/io/addrinfo.hpp>
#include <library/io/socket.hpp>

struct addrinfo;

namespace concrete {

class URL {
public:
	enum { MaxLength = 4096 };

	explicit URL(const char *url);

	const std::string &host() const throw () { return m_host; }
	const std::string &port() const throw () { return m_port; }
	const std::string &path() const throw () { return m_path; }

private:
	std::string m_host;
	std::string m_port;
	std::string m_path;
};

class LibraryURLOpener: public URLOpener {
protected:
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

public:
	LibraryURLOpener(const StringObject &url, Buffer *response, Buffer *request);
	virtual ~LibraryURLOpener() throw () {}

	virtual bool headers_received() { return m_state >= ReceivedHeaders; }
	virtual bool content_consumable();
	virtual bool content_received() { return m_state >= ReceivedContent; }

	virtual void suspend_until_headers_received() { suspend_until(ReceivedHeaders); }
	virtual void suspend_until_content_consumable() { suspend_until_content_received(); }
	virtual void suspend_until_content_received() { suspend_until(ReceivedContent); }

	virtual int  response_status() const { return m_response_status; }
	virtual long response_length() const { return m_response_length; }

protected:
	void suspend_until(State objective);

	State resolve();
	State resolving();
	State connect(const struct addrinfo *addrinfo);
	State connecting();
	State send_headers();
	State sending_headers();
	State send_content();
	State sending_content();
	State receive_headers();
	State receiving_headers();
	State receive_content();
	State receiving_content();
	State received_content();

	bool parse_headers();
	void parse_header(const char *line, size_t length);

	const URL                 m_url;
	State                     m_state;
	std::unique_ptr<AddrInfo> m_addrinfo;
	std::unique_ptr<Socket>   m_socket;
	Buffer                    m_request_headers;
	Buffer                   *m_request_content;
	size_t                    m_request_length;
	size_t                    m_request_sent;
	Buffer                   *m_response_buffer;
	unsigned int              m_response_status;
	long                      m_response_length;
};

} // namespace

#endif
