/*
 * Copyright (c) 2011, 2012  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef LIBRARY_API_HPP
#define LIBRARY_API_HPP

#define CONCRETE_API __attribute__ ((visibility ("default")))

#include "concrete.h"

#include <memory>

#include <concrete/context.hpp>

#include <library/event.hpp>
#include <library/libevent/event.hpp>

struct ConcreteContext {
	std::unique_ptr<concrete::LibeventLoop> event_loop;
	std::unique_ptr<concrete::Context>      context;
};

struct ConcreteEventCallback: public concrete::EventCallback {
public:
	ConcreteEventCallback(concrete::LibeventLoop       &event_loop,
	                      const concrete::EventTrigger &trigger,
	                      ConcreteEventFunction         function,
	                      void                         *data) throw ():
		m_event_loop(event_loop),
		m_trigger(trigger),
		m_function(function),
		m_data(data) {}

	virtual void resume() throw ();

private:
	concrete::LibeventLoop       &m_event_loop;
	const concrete::EventTrigger  m_trigger;
	const ConcreteEventFunction   m_function;
	void                   *const m_data;
};

#endif
