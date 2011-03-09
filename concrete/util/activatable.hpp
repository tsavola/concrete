/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_ACTIVE_HPP
#define CONCRETE_UTIL_ACTIVE_HPP

#include <cassert>
#include <cstdlib>

#include <concrete/util/noncopyable.hpp>
#include <concrete/util/thread.hpp>

namespace concrete {

template <typename Impl>
class activatable {
public:
	static Impl &Active()
	{
		assert(m_active);
		return *m_active;
	}

	void activate()
	{
		assert(m_active == NULL);
		m_active = static_cast<Impl *> (this);
	}

	void deactivate()
	{
		assert(m_active == this);
		m_active = NULL;
	}

private:
	static CONCRETE_THREAD_LOCAL Impl *m_active;
};

template <typename Impl>
CONCRETE_THREAD_LOCAL Impl *activatable<Impl>::m_active;

template <typename Activatable>
class active_scope: noncopyable {
public:
	explicit active_scope(Activatable &activatable): m_activatable(activatable)
	{
		m_activatable.activate();
	}

	~active_scope()
	{
		m_activatable.deactivate();
	}

private:
	Activatable &m_activatable;
};

} // namespace

#endif
