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

#include <string>

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

} // namespace

#endif
