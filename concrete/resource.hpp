/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_RESOURCE_HPP
#define CONCRETE_RESOURCE_HPP

#include <cstdint>
#include <exception>

#include <concrete/util/noncopyable.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

typedef uint32_t ResourceId;

class Resource: Noncopyable {
public:
	virtual ~Resource() throw ();
};

class ResourceError: public std::exception {
public:
	ResourceError() throw ();
	virtual ~ResourceError() throw ();

	virtual const char *what() const throw ();
};

class ResourceManager: Noncopyable {
	class Impl;

public:
	struct Snapshot {
		Portable<ResourceId> begin;
		Portable<ResourceId> end;

		Snapshot() throw ();
		Snapshot(ResourceId begin, ResourceId end) throw ();

	} CONCRETE_PACKED;

	template <typename ResourceType>
	struct Allocation {
		ResourceId id;
		ResourceType &resource;
	};

	ResourceManager();
	explicit ResourceManager(const Snapshot &snapshot);
	~ResourceManager() throw ();

	Snapshot snapshot() const throw ();

	template <typename ResourceType, typename... Args>
	Allocation<ResourceType> new_resource(Args... args);

	template <typename ResourceType>
	ResourceType &resource(ResourceId id) const;

	void delete_resource(ResourceId id) throw ();
	bool resource_lost(ResourceId id) const throw ();

	void wait_event(int fd, short events);
	void poll_events();

private:
	ResourceId append_resource(Resource *resource);
	Resource *find_resource(ResourceId id) const throw ();

	Impl *const m_impl;
};

} // namespace

#include "resource-inline.hpp"

#endif
