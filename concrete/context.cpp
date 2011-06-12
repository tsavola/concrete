/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "context.hpp"

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

namespace concrete {

Context::Snapshot::Snapshot() throw ()
{
}

Context::Snapshot::Snapshot(const Arena::Snapshot &arena,
                            const ResourceManager::Snapshot &resources,
                            BlockId system_objects) throw ():
	Arena::Snapshot(arena),
	resources(resources),
	system_objects(system_objects)
{
}

size_t Context::Snapshot::head_size() const throw ()
{
	return sizeof (Snapshot) - sizeof (Arena::Snapshot) + Arena::Snapshot::head_size();
}

Context::SystemObjectsBlock::SystemObjectsBlock(const NoneObject &none) throw ():
	none                    (none),
	type_type               (TypeObject::NewBuiltin(none)),
	object_type             (TypeObject::NewBuiltin(none, type_type)),
	none_type               (TypeObject::NewBuiltin(none, type_type)),
	string_type             (TypeObject::NewBuiltin(none, type_type)),
	long_type               (TypeObject::NewBuiltin(none, type_type)),
	bytes_type              (TypeObject::NewBuiltin(none, type_type)),
	tuple_type              (TypeObject::NewBuiltin(none, type_type)),
	tuple_empty             (none),
	dict_type               (TypeObject::NewBuiltin(none, type_type)),
	dict_empty              (none),
	code_type               (TypeObject::NewBuiltin(none, type_type)),
	function_type           (TypeObject::NewBuiltin(none, type_type)),
	internal_type           (TypeObject::NewBuiltin(none, type_type)),
	module_type             (TypeObject::NewBuiltin(none, type_type)),
	builtins                (none),
	modules                 (none)
{
}

Arena::Allocation Context::AllocBlock(size_t size)
{
	return Active().m_arena.alloc_block(size);
}

void Context::FreeBlock(BlockId id) throw ()
{
	Active().m_arena.free_block(id);
}

void Context::DeleteResource(ResourceId id) throw ()
{
	Active().m_resource_manager.delete_resource(id);
}

bool Context::ResourceLost(ResourceId id) throw ()
{
	return Active().m_resource_manager.resource_lost(id);
}

void Context::WaitEvent(int fd, short events)
{
	Active().m_resource_manager.wait_event(fd, events);
}

Context::SystemObjectsBlock *Context::SystemObjects()
{
	return Active().system_objects();
}

const Context::SystemObjectsBlock *Context::NonthrowingSystemObjects() throw ()
{
	auto &active = Active();
	return active.m_arena.nonthrowing_block_pointer<SystemObjectsBlock>(active.m_system_objects);
}

Object Context::LoadBuiltinName(const Object &name)
{
	return SystemObjects()->builtins->cast<DictObject>().get_item(name);
}

Object Context::ImportBuiltinModule(const Object &name)
{
	return SystemObjects()->modules->cast<DictObject>().get_item(name);
}

Context::Context()
{
	ContextScope scope(*this);

	auto none = NoneObject::NewBuiltin();

	m_system_objects = m_arena.new_block<SystemObjectsBlock>(none);

	none.init_builtin       (system_objects()->none_type);

	TypeObjectTypeInit      (system_objects()->type_type);
	ObjectTypeInit          (system_objects()->object_type);
	NoneObjectTypeInit      (system_objects()->none_type);
	StringObjectTypeInit    (system_objects()->string_type);
	LongObjectTypeInit      (system_objects()->long_type);
	BytesObjectTypeInit     (system_objects()->bytes_type);
	TupleObjectTypeInit     (system_objects()->tuple_type);
	DictObjectTypeInit      (system_objects()->dict_type);
	CodeObjectTypeInit      (system_objects()->code_type);
	FunctionObjectTypeInit  (system_objects()->function_type);
	InternalObjectTypeInit  (system_objects()->internal_type);
	ModuleObjectTypeInit    (system_objects()->module_type);

	auto modules = DictObject::New();

	auto builtins = BuiltinsModuleInit(modules);
	ConcreteModuleInit(modules);

	system_objects()->builtins = builtins;
	system_objects()->modules = modules;
}

Context::Context(const Snapshot &snapshot):
	m_arena(snapshot),
	m_resource_manager(snapshot.resources),
	m_system_objects(snapshot.system_objects)
{
}

Context::Snapshot Context::snapshot() const throw ()
{
	return Snapshot(m_arena.snapshot(),
	                m_resource_manager.snapshot(),
	                m_system_objects);
}

void Context::poll_events()
{
	m_resource_manager.poll_events();
}

Context::SystemObjectsBlock *Context::system_objects()
{
	return m_arena.block_pointer<SystemObjectsBlock>(m_system_objects);
}

} // namespace
