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
#include <map>

#include <concrete/pointer.hpp>
#include <concrete/portable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

class Resource: Noncopyable {
public:
	virtual ~Resource() throw () {}
};

class ResourceSlot: public Pointer {
	friend class Pointer;

public:
	static ResourceSlot New();
	static void Destroy(ResourceSlot *ptr) throw ();

	ResourceSlot() throw () {}
	ResourceSlot(const ResourceSlot &other) throw (): Pointer(other) {}

	void destroy() throw ();

protected:
	struct Data {} CONCRETE_PACKED;

	explicit ResourceSlot(unsigned int address) throw (): Pointer(address) {}
};

class ResourceManager {
public:
	static ResourceManager &Active() throw ();

	~ResourceManager() throw ();

	template <typename T, typename... Args> ResourceSlot new_resource(Args... args);
	void destroy_resource(ResourceSlot slot) throw ();
	bool is_resource_lost(ResourceSlot slot) const throw ();
	template <typename T> T *resource_cast(ResourceSlot slot) const;

private:
	typedef std::map<unsigned int, Resource *> Map;

	ResourceSlot add_resource(Resource *resource);
	Resource *find_resource(ResourceSlot slot) const throw ();

	Map m_map;
};

template <typename T>
class PortableResource {
public:
	template <typename... Args> static PortableResource New(Args... args);

	PortableResource() throw () {}
	PortableResource(const PortableResource &other) throw (): m_slot(other.m_slot) {}

	void operator=(const PortableResource &other) throw () { m_slot = other.m_slot; }

	void destroy() throw ();
	bool is_lost() const throw ();

	operator bool() const throw () { return bool(m_slot); }
	bool operator!() const throw () { return !m_slot; }
	T *operator*() const throw ();
	T *operator->() const throw () { return operator*(); }

private:
	explicit PortableResource(const ResourceSlot &slot) throw (): m_slot(slot) {}

	Portable<ResourceSlot> m_slot;
} CONCRETE_PACKED;

template <typename T>
struct ResourceCreate {
	template <typename... Args> static T *New(Args... args) { return new T(args...); }
};

class ResourceError: public std::exception {
public:
	ResourceError() throw ();
	virtual ~ResourceError() throw () {}

	virtual const char *what() const throw ();
};

} // namespace

#include "resource-inline.hpp"

#endif
