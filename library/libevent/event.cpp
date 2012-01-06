/*
 * Copyright (c) 2011, 2012  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "event.hpp"

#include <cstddef>
#include <stdexcept>

#include <concrete/util/trace.hpp>

#include <library/event.hpp>

namespace concrete {

LibeventLoop::LibeventLoop():
	m_base(event_base_new())
{
	if (m_base == NULL)
		throw std::runtime_error("event_base_new()");
}

LibeventLoop::~LibeventLoop() throw ()
{
	event_base_free(m_base);
}

void LibeventLoop::wait(const EventTrigger &trigger, EventCallback *arg)
{
	int fd = trigger.fd;
	short events = 0;

	if (trigger.conditions & EventFileReadable)
		events |= EV_READ;

	if (trigger.conditions & EventFileWritable)
		events |= EV_WRITE;

	if (event_base_once(m_base, fd, events, callback, arg, NULL) < 0)
		throw std::runtime_error("event_base_once()");

	Trace("suspended");
}

void LibeventLoop::poll(bool nonblocking)
{
	int flags = EVLOOP_ONCE;

	if (nonblocking)
		flags |= EVLOOP_NONBLOCK;

	if (event_base_loop(m_base, flags) < 0)
		throw std::runtime_error("event_base_loop()");
}

void LibeventLoop::callback(int fd, short events, void *arg) throw ()
{
	Trace("event: fd=%d", fd);

	reinterpret_cast<EventCallback *> (arg)->resume();
}

} // namespace
