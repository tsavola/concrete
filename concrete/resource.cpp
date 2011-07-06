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
#include <memory>
#include <utility>

#include <concrete/context.hpp>
#include <concrete/util/backtrace.hpp>

namespace concrete {

ResourceSlot ResourceSlot::New()
{
	return NewPointer<ResourceSlot>();
}

void ResourceSlot::Destroy(ResourceSlot *ptr) throw ()
{
	ptr->destroy();
}

void ResourceSlot::destroy() throw ()
{
	DestroyPointer(*this);
}

unsigned int ResourceSlot::key() const throw ()
{
	return address();
}

ResourceManager &ResourceManager::Active() throw ()
{
	return Context::Active().resource_manager();
}

ResourceManager::~ResourceManager() throw ()
{
	for (auto i = m_map.begin(); i != m_map.end(); ++i)
		delete i->second;
}

ResourceSlot ResourceManager::add_resource(Resource *resource)
{
	ResourceSlot slot = ResourceSlot::New();

	std::unique_ptr<ResourceSlot, void (*)(ResourceSlot *)> slot_deleter(
		&slot, ResourceSlot::Destroy);

	m_map[slot.key()] = resource;

	slot_deleter.release();

	return slot;
}

Resource *ResourceManager::find_resource(ResourceSlot slot) const throw ()
{
	auto i = m_map.find(slot.key());
	if (i != m_map.end())
		return i->second;

	return NULL;
}

void ResourceManager::destroy_resource(ResourceSlot slot) throw ()
{
	if (slot) {
		auto i = m_map.find(slot.key());
		if (i != m_map.end()) {
			delete i->second;
			m_map.erase(i);
		}

		slot.destroy();
	}
}

bool ResourceManager::is_resource_lost(ResourceSlot slot) const throw ()
{
	return m_map.find(slot.key()) == m_map.end();
}

ResourceError::ResourceError() throw ()
{
	Backtrace();
}

const char *ResourceError::what() const throw ()
{
	return "Resource access error";
}

} // namespace
