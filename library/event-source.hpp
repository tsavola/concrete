/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_EVENT_SOURCE_HPP
#define LIBRARY_EVENT_SOURCE_HPP

#include <concrete/event.hpp>

namespace concrete {

class EventSource {
public:
	EventSource(int fd) throw (): m_fd(fd) {}
	operator int() const throw () { return m_fd; }

private:
	int m_fd;
};

} // namespace

#endif
