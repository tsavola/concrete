/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_CONTEXT_HPP
#define CONCRETE_CONTEXT_HPP

#include <cassert>
#include <cstddef>

#include <concrete/arena-access.hpp>
#include <concrete/arena.hpp>
#include <concrete/objects/none.hpp>
#include <concrete/objects/object-partial.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/portable.hpp>
#include <concrete/resource.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

class EventCallback;
class EventLoop;
class EventSource;
class Execution;
class ScopedContext;

class Context: ArenaAccess {
	friend class EventCallback;
	friend class ScopedContext;

public:
	struct Data;

	static Context &Active() throw ();

	explicit Context(EventLoop &loop);
	Context(EventLoop &loop, void *base, size_t size);

	bool is_active() const throw () { return m_active; }

	Arena &arena() throw () { return m_arena; }
	ResourceManager &resource_manager() throw () { return m_resource_manager; }

	Data *data();
	const Data *nonthrowing_data() throw ();

	const Pointer &none_pointer() const throw () { return m_none_pointer; }

	Object load_builtin_name(const Object &name);
	Object import_builtin_module(const Object &name);

	void add_execution(const Execution &execution);
	Portable<Execution> &execution() throw ();

	void suspend_until(const EventSource &source, unsigned int conditions);

	bool executable() throw ();
	void execute();

private:
	EventLoop       &m_event_loop;
	Arena            m_arena;
	ResourceManager  m_resource_manager;
	Pointer          m_none_pointer;
	bool             m_active;
};

class ScopedContext: Noncopyable {
public:
	explicit ScopedContext(Context &context) throw ();
	~ScopedContext() throw ();

private:
	Context &m_context;
};

} // namespace

#endif
