/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_IO_HTTP_HPP
#define CONCRETE_IO_HTTP_HPP

#include <cstddef>
#include <string>

#include <concrete/io/buffer.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/resource.hpp>

namespace concrete {

namespace HTTP {
	enum Method {
		GET  = 1,
		POST = 3,
	};

	enum Status {
		OK = 200,
	};
}

class HTTPTransaction: public Resource {
public:
	void set_request_length(size_t length);
	void reset_request_buffer(Buffer *buffer = NULL);
	void reset_response_buffer(Buffer *buffer = NULL);

	HTTP::Status response_status() const;
	long response_length() const;

	bool connected();
	bool headers_sent();
	bool content_producable();
	bool content_sent();
	bool headers_received();
	bool content_consumable();
	bool content_received();

	void suspend_until_connected();
	void suspend_until_headers_sent();
	void suspend_until_content_producable();
	void suspend_until_content_sent();
	void suspend_until_headers_received();
	void suspend_until_content_consumable();
	void suspend_until_content_received();
};

template <> struct ResourceCreate<HTTPTransaction> {
	static HTTPTransaction *New(HTTP::Method method,
	                            const StringObject &url,
	                            size_t request_length = 0);
};

} // namespace

#endif
