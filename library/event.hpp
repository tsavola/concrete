/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_EVENT_HPP
#define LIBRARY_EVENT_HPP

#include <concrete/event.hpp>

namespace concrete {

enum EventCondition {
	EventFileReadable = 1,
	EventFileWritable = 2,
};

class EventTrigger {
public:
	EventTrigger(int fd, unsigned int conditions) throw (): fd(fd), conditions(conditions) {}

	const int          fd;
	const unsigned int conditions;
};

} // namespace

#endif
