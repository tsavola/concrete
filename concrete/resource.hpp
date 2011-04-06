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

#include <cstddef>
#include <cstdint>
#include <memory>

#include <concrete/util/backtrace.hpp>
#include <concrete/util/id.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

typedef uint32_t ResourceKey;

typedef IdOps<ResourceKey>         ResourceIdOps;
typedef PortableIdOps<ResourceKey> PortableResourceIdOps;

typedef IdLogic<ResourceKey, ResourceIdOps>         ResourceId;
typedef IdLogic<ResourceKey, PortableResourceIdOps> PortableResourceId;

struct ResourceSnapshot {
	Portable<ResourceKey> begin;
	Portable<ResourceKey> end;

	ResourceSnapshot() throw ():
		begin(0),
		end(0)
	{
	}

	ResourceSnapshot(ResourceKey begin, ResourceKey end) throw ():
		begin(begin),
		end(end)
	{
	}
} CONCRETE_PACKED;

class Resource: Noncopyable {
public:
	virtual ~Resource() throw ()
	{
	}
};

class ResourceError: public std::exception {
public:
	ResourceError() throw ()
	{
		Backtrace();
	}

	virtual ~ResourceError() throw ()
	{
	}

	virtual const char *what() const throw ()
	{
		return "Resource access error";
	}
};

class ResourceManager: Noncopyable {
	class Impl;

public:
	template <typename T>
	struct Allocation {
		ResourceId id;
		T &resource;
	};

	ResourceManager();
	explicit ResourceManager(const ResourceSnapshot &snapshot);
	~ResourceManager() throw ();

	ResourceSnapshot snapshot() const throw ();

	template <typename T, typename... Args>
	Allocation<T> new_resource(Args... args)
	{
		std::auto_ptr<T> resource(new T(args...));
		auto id = append_resource(resource.get());
		auto ptr = resource.get();
		resource.release();
		return Allocation<T> { id, *ptr };
	}

	template <typename T>
	T &resource(ResourceId id) const
	{
		auto plain = find_resource(id);
		if (plain == NULL)
			throw ResourceError();

		auto typed = dynamic_cast<T *> (plain);
		if (typed == NULL)
			throw ResourceError();

		return *typed;
	}

	void delete_resource(ResourceId id) throw ();
	bool resource_lost(ResourceId id) const throw ();

private:
	ResourceId append_resource(Resource *resource);
	Resource *find_resource(ResourceId id) const throw ();

	Impl *const m_impl;
};

} // namespace

#endif
