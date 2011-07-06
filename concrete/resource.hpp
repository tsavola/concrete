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

struct event_base;

namespace concrete {

class ResourceError: public std::exception {
public:
	ResourceError() throw ();
	virtual ~ResourceError() throw () {}

	virtual const char *what() const throw ();
};

class ResourceSlot: public Pointer {
	friend class Pointer;

public:
	static ResourceSlot New();
	static void Destroy(ResourceSlot *ptr) throw ();

	ResourceSlot() throw () {}
	ResourceSlot(const ResourceSlot &other) throw (): Pointer(other) {}

	void destroy() throw ();
	unsigned int key() const throw ();

protected:
	struct Data {} CONCRETE_PACKED;

	explicit ResourceSlot(unsigned int address) throw (): Pointer(address) {}
};

class ResourceManager {
public:
	class EventCallback: Noncopyable {
	public:
		virtual void resume() throw () = 0;
	};

	static ResourceManager &Active() throw ();

	ResourceManager();
	~ResourceManager() throw ();

	template <typename T, typename... Args> ResourceSlot new_resource(Args... args);
	void destroy_resource(ResourceSlot slot) throw ();
	bool is_resource_lost(ResourceSlot slot) const throw ();
	template <typename T> T *resource_cast(ResourceSlot slot) const;

	void wait_event(int fd, short events, EventCallback *callback);
	void poll_events();

private:
	class VirtualWrap: Noncopyable {
	public:
		virtual ~VirtualWrap() throw () {}
	};

	template <typename T>
	class Wrap: public VirtualWrap {
	public:
		explicit Wrap(T *resource) throw ();
		virtual ~Wrap() throw ();

		T *const resource;
	};

	typedef std::map<unsigned int, VirtualWrap *> WrapMap;

	static void event_callback(int fd, short events, void *arg);

	ResourceSlot add_resource(VirtualWrap *wrap);
	VirtualWrap *find_resource(ResourceSlot slot) const throw ();

	WrapMap                  m_wrap_map;
	struct event_base *const m_event_base;
};

template <typename T>
class PortableResource {
public:
	~PortableResource() throw ();

	template <typename... Args> void create(Args... args);
	void destroy() throw ();
	bool is_lost() const throw ();

	operator bool() const throw ();
	bool operator!() const throw ();
	T *operator*() const throw ();
	T *operator->() const throw ();

private:
	Portable<ResourceSlot> m_slot;
} CONCRETE_PACKED;

} // namespace

#include "resource-inline.hpp"

#endif
