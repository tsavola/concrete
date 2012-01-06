/*
 * Copyright (c) 2011, 2012  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_LIBEVENT_EVENT_HPP
#define LIBRARY_LIBEVENT_EVENT_HPP

#include <event.h>

#include <concrete/event.hpp>

struct evdns_base;

namespace concrete {

class LibeventLoop: public EventLoop {
public:
	LibeventLoop();
	virtual ~LibeventLoop() throw ();

	virtual void wait(const EventTrigger &trigger, EventCallback *callback);
	void poll(bool nonblocking);

	struct evdns_base *new_evdns_base();

private:
	static void callback(int fd, short events, void *arg) throw ();

	struct event_base *const m_base;
};

} // namespace

#endif
