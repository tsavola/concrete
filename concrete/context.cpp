/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "context-data.hpp"

#include <cstdlib>
#include <memory>

#include <concrete/event.hpp>
#include <concrete/execution.hpp>
#include <concrete/modules/builtins.hpp>
#include <concrete/modules/concrete.hpp>
#include <concrete/objects/bool.hpp>
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
#include <concrete/util/assert.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/thread.hpp>

namespace concrete {

class ContextCallback: public EventCallback {
	friend class EventCallback;

public:
	ContextCallback(Context &context, const Execution &execution) throw ();

private:
	Context         &m_context;
	const Execution  m_execution;
};

static CONCRETE_THREAD_LOCAL Context *active_context;

Context::Data::Data(const NoneObject &none,
                    const TypeObject &type_type,
                    const TypeObject &object_type,
                    const TypeObject &none_type,
                    const TypeObject &string_type,
                    const TypeObject &long_type,
                    const TypeObject &bool_type,
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
	bool_type     (bool_type),
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

Context &Context::Active() throw ()
{
	assert(active_context);
	return *active_context;
}

Context::Context(EventLoop &loop):
	m_event_loop(loop)
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
	auto bool_type     = TypeObject::NewBuiltin(type_type);
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
	                  bool_type,
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
	BoolObjectTypeInit     (data()->bool_type);
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

Context::Context(EventLoop &loop, void *base, size_t size):
	m_event_loop(loop),
	m_arena(base, size)
{
	ScopedContext activate(*this);

	m_none_pointer = *data()->none;

	if (*data()->current) {
		data()->runnable.append(data()->current);
		data()->current = Execution();
	}
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

void Context::suspend_until(const EventTrigger &trigger)
{
	std::unique_ptr<ContextCallback> callback(new ContextCallback(*this, data()->current));
	m_event_loop.wait(trigger, callback.get());
	callback.release();

	data()->waiting.append(data()->current);
	data()->current = Execution();
}

bool Context::executable() throw ()
{
	// error condition
	if (*data()->current)
		return false;

	return data()->runnable || data()->waiting;
}

bool Context::execute()
{
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
	}

	return bool(data()->runnable);
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

ContextCallback::ContextCallback(Context &context, const Execution &execution) throw ():
	m_context(context),
	m_execution(execution)
{
}

void EventCallback::resume() throw ()
{
	auto callback = static_cast<ContextCallback *> (this);

	callback->m_context.data()->waiting.remove(callback->m_execution);
	callback->m_context.data()->runnable.append(callback->m_execution);

	delete callback;
}

} // namespace
