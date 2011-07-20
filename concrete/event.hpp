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

class EventTrigger;

class EventCallback: Noncopyable {
public:
	void resume() throw ();
};

class EventLoop: Noncopyable {
public:
	virtual ~EventLoop() throw () {}

	virtual void wait(const EventTrigger &trigger, EventCallback *callback) = 0;
	virtual void poll() = 0;
};

} // namespace

#endif
