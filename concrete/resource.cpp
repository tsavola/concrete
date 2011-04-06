/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "resource.hpp"

#include <cassert>
#include <map>
#include <utility>

#include <event.h>

#include <concrete/util/trace.hpp>

namespace concrete {

class ResourceManager::Impl {
	typedef std::map<ResourceKey, Resource *> ResourceMap;

public:
	Impl():
		m_lost_begin(0),
		m_lost_end(0),
		m_event_base(event_base_new()),
		m_suspended(false)
	{
		if (m_event_base == NULL)
			throw ResourceError();
	}

	Impl(const ResourceSnapshot &snapshot):
		m_lost_begin(snapshot.begin),
		m_lost_end(snapshot.end),
		m_event_base(event_base_new()),
		m_suspended(false)
	{
		if (m_event_base == NULL)
			throw ResourceError();
	}

	~Impl() throw ()
	{
		for (auto i = m_resources.begin(); i != m_resources.end(); ++i)
			delete i->second;

		event_base_free(m_event_base);
	}

	ResourceSnapshot snapshot() const throw ()
	{
		ResourceKey begin = m_lost_begin;
		ResourceKey end = m_lost_end;

		if (begin == end) {
			if (m_resources.empty()) {
				begin = 0;
				end = 0;
			} else {
				begin = m_resources.begin()->first;
				end = m_resources.rbegin()->first + 1;
			}
		} else {
			if (!m_resources.empty())
				end = m_resources.rbegin()->first + 1;
		}

		return ResourceSnapshot(begin, end);
	}

	ResourceId append_resource(Resource *resource)
	{
		typedef std::pair<ResourceKey, Resource *> Pair;

		ResourceKey key = 0;
		auto i = m_resources.end();

		if (!m_resources.empty()) {
			--i;
			key = i->first + 1;
		}

		m_resources.insert(i, Pair(key, resource));

		return ResourceId::New(key);
	}

	Resource *find_resource(ResourceId id) const throw ()
	{
		auto i = m_resources.find(id.value());
		if (i == m_resources.end())
			return NULL;

		return i->second;
	}

	void delete_resource(ResourceId id) throw ()
	{
		if (id == NULL)
			return;

		auto i = m_resources.find(id.value());
		if (i == m_resources.end())
			return;

		delete i->second;
		m_resources.erase(i);
	}

	bool resource_lost(ResourceId id) const throw ()
	{
		if (id == NULL)
			return false;

		return id.value() >= m_lost_begin && id.value() < m_lost_end;
	}

	void wait_event(int fd, short events)
	{
		assert(!m_suspended);

		if (event_base_once(m_event_base, fd, events, event_callback, this, NULL) < 0)
			throw ResourceError();

		m_suspended = true;

		ConcreteTrace(("suspended"));
	}

	void poll_events()
	{
		if (m_suspended && event_base_loop(m_event_base, EVLOOP_ONCE) < 0)
			throw ResourceError();
	}

private:
	static void event_callback(int fd, short events, void *arg)
	{
		auto impl = reinterpret_cast<Impl *> (arg);

		assert(impl->m_suspended);
		impl->m_suspended = false;

		ConcreteTrace(("resumed"));
	}

	const ResourceKey m_lost_begin;
	const ResourceKey m_lost_end;
	ResourceMap m_resources;
	struct event_base *const m_event_base;
	bool m_suspended;
};

ResourceManager::ResourceManager():
	m_impl(new Impl)
{
}

ResourceManager::ResourceManager(const ResourceSnapshot &snapshot):
	m_impl(new Impl(snapshot))
{
}

ResourceManager::~ResourceManager() throw ()
{
	delete m_impl;
}

ResourceSnapshot ResourceManager::snapshot() const throw ()
{
	return m_impl->snapshot();
}

ResourceId ResourceManager::append_resource(Resource *resource)
{
	return m_impl->append_resource(resource);
}

Resource *ResourceManager::find_resource(ResourceId id) const throw ()
{
	return m_impl->find_resource(id);
}

void ResourceManager::delete_resource(ResourceId id) throw ()
{
	m_impl->delete_resource(id);
}

bool ResourceManager::resource_lost(ResourceId id) const throw ()
{
	return m_impl->resource_lost(id);
}

void ResourceManager::wait_event(int fd, short events)
{
	m_impl->wait_event(fd, events);
}

void ResourceManager::poll_events()
{
	m_impl->poll_events();
}

} // namespace
