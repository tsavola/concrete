/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "context-data.hpp"

#include <cassert>
#include <cstdlib>
#include <memory>

#include <concrete/execution.hpp>
#include <concrete/modules/builtins.hpp>
#include <concrete/modules/concrete.hpp>
#include <concrete/objects/bytes.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/function.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/thread.hpp>

namespace concrete {

class Context::EventCallback: public ResourceManager::EventCallback {
public:
	EventCallback(Context &context, const Execution &execution) throw ();

	virtual void resume() throw ();

private:
	Context &m_context;
	const Execution m_execution;
};

static CONCRETE_THREAD_LOCAL Context *active_context;

Context::Data::Data(const NoneObject &none,
                          const TypeObject &type_type,
                          const TypeObject &object_type,
                          const TypeObject &none_type,
                          const TypeObject &string_type,
                          const TypeObject &long_type,
                          const TypeObject &bytes_type,
                          const TypeObject &tuple_type,
                          const TypeObject &dict_type,
                          const TypeObject &code_type,
                          const TypeObject &function_type,
                          const TypeObject &internal_type,
                          const TypeObject &module_type) throw ():
	none          (none),
	type_type     (type_type),
	object_type   (object_type),
	none_type     (none_type),
	string_type   (string_type),
	long_type     (long_type),
	bytes_type    (bytes_type),
	tuple_type    (tuple_type),
	tuple_empty   (none),
	dict_type     (dict_type),
	code_type     (code_type),
	function_type (function_type),
	internal_type (internal_type),
	module_type   (module_type)
{
}

Context::EventCallback::EventCallback(Context &context, const Execution &execution) throw ():
	m_context(context),
	m_execution(execution)
{
}

void Context::EventCallback::resume() throw ()
{
	m_context.data()->waiting.remove(m_execution);
	m_context.data()->runnable.append(m_execution);

	delete this;
}

Context &Context::Active() throw ()
{
	assert(active_context);
	return *active_context;
}

Context::Context()
{
	ScopedContext activate(*this);

	auto context_address = m_arena.allocate(sizeof (Data)).address;
	assert(context_address == Arena::InitialAllocationAddress);

	auto none = NoneObject::NewBuiltin();
	m_none_pointer = none;

	auto type_type     = TypeObject::NewBuiltin();
	auto object_type   = TypeObject::NewBuiltin(type_type);
	auto none_type     = TypeObject::NewBuiltin(type_type);
	auto string_type   = TypeObject::NewBuiltin(type_type);
	auto long_type     = TypeObject::NewBuiltin(type_type);
	auto bytes_type    = TypeObject::NewBuiltin(type_type);
	auto tuple_type    = TypeObject::NewBuiltin(type_type);
	auto dict_type     = TypeObject::NewBuiltin(type_type);
	auto code_type     = TypeObject::NewBuiltin(type_type);
	auto function_type = TypeObject::NewBuiltin(type_type);
	auto internal_type = TypeObject::NewBuiltin(type_type);
	auto module_type   = TypeObject::NewBuiltin(type_type);

	new (data()) Data(none,
	                        type_type,
	                        object_type,
	                        none_type,
	                        string_type,
	                        long_type,
	                        bytes_type,
	                        tuple_type,
	                        dict_type,
	                        code_type,
	                        function_type,
	                        internal_type,
	                        module_type);

	none.init_builtin      (data()->none_type);

	TypeObjectTypeInit     (data()->type_type);
	ObjectTypeInit         (data()->object_type);
	NoneObjectTypeInit     (data()->none_type);
	StringObjectTypeInit   (data()->string_type);
	LongObjectTypeInit     (data()->long_type);
	BytesObjectTypeInit    (data()->bytes_type);
	TupleObjectTypeInit    (data()->tuple_type);
	DictObjectTypeInit     (data()->dict_type);
	CodeObjectTypeInit     (data()->code_type);
	FunctionObjectTypeInit (data()->function_type);
	InternalObjectTypeInit (data()->internal_type);
	ModuleObjectTypeInit   (data()->module_type);

	auto modules = DictObject::New();

	auto builtins = BuiltinsModuleInit(modules);
	ConcreteModuleInit(modules);

	data()->builtins = builtins;
	data()->modules = modules;
}

Context::Context(void *base, size_t size):
	m_arena(base, size)
{
	ScopedContext activate(*this);

	m_none_pointer = *data()->none;
}

Object Context::load_builtin_name(const Object &name)
{
	return data()->builtins->cast<DictObject>().get_item(name);
}

Object Context::import_builtin_module(const Object &name)
{
	return data()->modules->cast<DictObject>().get_item(name);
}

void Context::add_execution(const Execution &execution)
{
	data()->runnable.append(execution);
}

Portable<Execution> &Context::execution() throw ()
{
	return data()->current;
}

void Context::wait_event(int fd, short events)
{
	std::unique_ptr<EventCallback> callback(new EventCallback(*this, data()->current));
	Active().resource_manager().wait_event(fd, events, callback.get());
	callback.release();

	data()->waiting.append(data()->current);
	data()->current = Execution();
}

bool Context::executable() throw ()
{
	assert(!data()->current);

	return data()->runnable || data()->waiting;
}

void Context::execute()
{
	while (executable()) {
		data()->current = data()->runnable.head();

		if (*data()->current) {
			data()->runnable.remove(data()->current);

			if (data()->current->execute()) {
				if (*data()->current)
					data()->runnable.append(data()->current);
			} else {
				assert(*data()->current);
				data()->current->destroy();
			}

			data()->current = Execution();
		} else {
			assert(data()->waiting);
			m_resource_manager.poll_events();
		}
	}
}

Context::Data *Context::data()
{
	return static_cast<Data *> (arena_access(Arena::InitialAllocationAddress, sizeof (Data)));
}

const Context::Data *Context::nonthrowing_data() throw ()
{
	return static_cast<Data *> (
		nonthrowing_arena_access(Arena::InitialAllocationAddress, sizeof (Data)));
}

ScopedContext::ScopedContext(Context &context) throw ():
	m_context(context)
{
	assert(active_context == NULL);

	m_context.m_active = true;
	active_context = static_cast<Context *> (&m_context);
}

ScopedContext::~ScopedContext() throw ()
{
	assert(active_context == &m_context);

	active_context = NULL;
	m_context.m_active = false;
}

} // namespace
