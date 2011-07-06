/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIB_LIBEVENT_HPP
#define LIB_LIBEVENT_HPP

#include <event.h>

#include <concrete/event.hpp>

namespace concrete {

class LibeventLoop: public EventLoop {
public:
	LibeventLoop();
	virtual ~LibeventLoop() throw ();

	virtual void wait(const EventSource &source,
	                  unsigned int conditions,
	                  EventCallback *callback);

	virtual void poll();

private:
	static void callback(int fd, short events, void *arg) throw ();

	struct event_base *const m_base;
};

} // namespace

#endif