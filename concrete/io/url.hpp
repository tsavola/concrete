/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_IO_URL_HPP
#define CONCRETE_IO_URL_HPP

#include <cstddef>

#include <concrete/io/buffer.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/resource.hpp>

namespace concrete {

class URLOpener: public Resource {
public:
	static URLOpener *New(const StringObject &url, Buffer *response, Buffer *request = NULL);

	virtual bool headers_received() = 0;
	virtual bool content_consumable() = 0;
	virtual bool content_received() = 0;

	virtual void suspend_until_headers_received() = 0;
	virtual void suspend_until_content_consumable() = 0;
	virtual void suspend_until_content_received() = 0;

	virtual int  response_status() const = 0;
	virtual long response_length() const = 0;
};

} // namespace

#endif
