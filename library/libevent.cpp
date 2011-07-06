/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "libevent.hpp"

#include <cstddef>
#include <stdexcept>

#include <concrete/util/trace.hpp>

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

void LibeventLoop::wait(const EventSource &source, unsigned int conditions, EventCallback *arg)
{
	int fd = source;
	short events = 0;

	if (conditions & EventSourceReadable)
		events |= EV_READ;

	if (conditions & EventSourceWritable)
		events |= EV_WRITE;

	if (event_base_once(m_base, fd, events, callback, arg, NULL) < 0)
		throw std::runtime_error("event_base_once()");

	Trace("suspended");
}

void LibeventLoop::poll()
{
	if (event_base_loop(m_base, EVLOOP_ONCE) < 0)
		throw std::runtime_error("event_base_loop()");
}

void LibeventLoop::callback(int fd, short events, void *arg) throw ()
{
	Trace("resumed");

	reinterpret_cast<EventCallback *> (arg)->resume();
}

} // namespace
