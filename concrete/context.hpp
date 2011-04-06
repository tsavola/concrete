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

#include <concrete/arena.hpp>
#include <concrete/block.hpp>
#include <concrete/objects/none-decl.hpp>
#include <concrete/objects/object-decl.hpp>
#include <concrete/objects/type-decl.hpp>
#include <concrete/resource.hpp>
#include <concrete/util/activatable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

struct ContextSnapshot: ArenaSnapshot {
	PortableBlockId system_objects;
	ResourceSnapshot resources;

	ContextSnapshot() throw ()
	{
	}

	ContextSnapshot(const ArenaSnapshot &arena,
	                BlockId system_objects,
	                const ResourceSnapshot &resources) throw ():
		ArenaSnapshot(arena),
		system_objects(system_objects),
		resources(resources)
	{
	}

	size_t head_size() const throw ()
	{
		return sizeof (ContextSnapshot) - sizeof (ArenaSnapshot) + ArenaSnapshot::head_size();
	}
} CONCRETE_PACKED;

class Context: public Activatable<Context>, Noncopyable {
public:
	struct SystemObjectsBlock: Block {
		const PortableNoneObject none;
		const PortableTypeObject type_type;
		const PortableTypeObject object_type;
		const PortableTypeObject none_type;
		const PortableTypeObject string_type;
		const PortableTypeObject long_type;
		const PortableTypeObject bytes_type;
		const PortableTypeObject tuple_type;
		PortableObject           tuple_empty;
		const PortableTypeObject dict_type;
		PortableObject           dict_empty;
		const PortableTypeObject code_type;
		const PortableTypeObject function_type;
		const PortableTypeObject internal_type;
		const PortableTypeObject module_type;
		PortableObject           builtins;
		PortableObject           modules;

		SystemObjectsBlock(const NoneObject &none) throw ();

	private:
		SystemObjectsBlock(const SystemObjectsBlock &);
		const SystemObjectsBlock &operator=(const SystemObjectsBlock &);

	} CONCRETE_PACKED;

	static Arena::Allocation AllocBlock(size_t size)
	{
		return Active().m_arena.alloc_block(size);
	}

	static void FreeBlock(BlockId id) throw ()
	{
		Active().m_arena.free_block(id);
	}

	template <typename T, typename... Args>
	static BlockId NewBlock(Args... args)
	{
		return Active().m_arena.new_block<T>(args...);
	}

	template <typename T, typename... Args>
	static BlockId NewCustomSizeBlock(size_t size, Args... args)
	{
		return Active().m_arena.new_custom_size_block<T>(size, args...);
	}

	template <typename T>
	static void DeleteBlock(BlockId id) // doesn't throw unless ~T() does
	{
		Active().m_arena.delete_block<T>(id);
	}

	template <typename T>
	static T *BlockPointer(BlockId id)
	{
		return Active().m_arena.block_pointer<T>(id);
	}

	template <typename T>
	static T *NonthrowingBlockPointer(BlockId id) throw ()
	{
		return Active().m_arena.nonthrowing_block_pointer<T>(id);
	}

	static SystemObjectsBlock *SystemObjects()
	{
		return Active().system_objects();
	}

	static const SystemObjectsBlock *NonthrowingSystemObjects() throw ()
	{
		auto &active = Active();
		return active.m_arena.nonthrowing_block_pointer<SystemObjectsBlock>(
			active.m_system_objects);
	}

	static Object LoadBuiltinName(const Object &name);
	static Object ImportBuiltinModule(const Object &name);

	template <typename T, typename... Args>
	static ResourceManager::Allocation<T> NewResource(Args... args)
	{
		return Active().m_resource_manager.new_resource<T>(args...);
	}

	template <typename T, typename... Args>
	static T &Resource(ResourceId id)
	{
		return Active().m_resource_manager.resource<T>(id);
	}

	static void DeleteResource(ResourceId id) throw ()
	{
		Active().m_resource_manager.delete_resource(id);
	}

	static bool ResourceLost(ResourceId id) throw ()
	{
		return Active().m_resource_manager.resource_lost(id);
	}

	Context();

	explicit Context(const ContextSnapshot &snapshot):
		m_arena(snapshot),
		m_system_objects(snapshot.system_objects),
		m_resource_manager(snapshot.resources)
	{
	}

	ContextSnapshot snapshot() const throw ()
	{
		return ContextSnapshot(m_arena.snapshot(),
		                       m_system_objects,
		                       m_resource_manager.snapshot());
	}

private:
	SystemObjectsBlock *system_objects()
	{
		return m_arena.block_pointer<SystemObjectsBlock>(m_system_objects);
	}

	Arena m_arena;
	BlockId m_system_objects;
	ResourceManager m_resource_manager;
};

typedef ActiveScope<Context> ContextScope;

} // namespace

#endif
