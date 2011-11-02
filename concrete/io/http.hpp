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
	static HTTPTransaction *New(HTTP::Method method,
	                            const StringObject &url,
	                            Buffer *request_content = NULL);

	virtual void reset_response_buffer(Buffer *buffer) = 0;

	virtual HTTP::Status response_status() const = 0;
	virtual long response_length() const = 0;

	virtual bool headers_received() = 0;
	virtual bool content_consumable() = 0;
	virtual bool content_received() = 0;

	virtual void suspend_until_headers_received() = 0;
	virtual void suspend_until_content_consumable() = 0;
	virtual void suspend_until_content_received() = 0;
};

} // namespace

#endif
