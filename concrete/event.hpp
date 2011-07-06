/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_EVENT_HPP
#define CONCRETE_EVENT_HPP

#include <concrete/util/noncopyable.hpp>

namespace concrete {

enum EventCondition {
	EventSourceReadable = 1,
	EventSourceWritable = 2,
};

class EventSource {
public:
	EventSource(int fd) throw (): m_fd(fd) {}
	operator int() const throw () { return m_fd; }

private:
	int m_fd;
};

class EventCallback: Noncopyable {
public:
	void resume() throw ();
};

class EventLoop: Noncopyable {
public:
	virtual ~EventLoop() throw () {}

	virtual void wait(const EventSource &source,
	                  unsigned int conditions,
	                  EventCallback *callback) = 0;

	virtual void poll() = 0;
};

} // namespace

#endif
