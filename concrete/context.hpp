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
#include <concrete/objects/none.hpp>
#include <concrete/objects/object-partial.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/resource.hpp>
#include <concrete/util/activatable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

class Context: public Activatable<Context>, Noncopyable {
	friend class IntegrityError;

public:
	struct Snapshot: Arena::Snapshot {
		ResourceManager::Snapshot resources;
		Portable<BlockId> system_objects;

		Snapshot() throw ();

		Snapshot(const Arena::Snapshot &arena,
		         const ResourceManager::Snapshot &resources,
		         BlockId system_objects) throw ();

		size_t head_size() const throw ();

	} CONCRETE_PACKED;

	struct SystemObjectsBlock: Block {
		const Portable<NoneObject> none;
		const Portable<TypeObject> type_type;
		const Portable<TypeObject> object_type;
		const Portable<TypeObject> none_type;
		const Portable<TypeObject> string_type;
		const Portable<TypeObject> long_type;
		const Portable<TypeObject> bytes_type;
		const Portable<TypeObject> tuple_type;
		Portable<Object>           tuple_empty;
		const Portable<TypeObject> dict_type;
		Portable<Object>           dict_empty;
		const Portable<TypeObject> code_type;
		const Portable<TypeObject> function_type;
		const Portable<TypeObject> internal_type;
		const Portable<TypeObject> module_type;
		Portable<Object>           builtins;
		Portable<Object>           modules;

		SystemObjectsBlock(const NoneObject &none) throw ();

	private:
		SystemObjectsBlock(const SystemObjectsBlock &);

		const SystemObjectsBlock &operator=(const SystemObjectsBlock &);

	} CONCRETE_PACKED;

	static Arena::Allocation AllocBlock(size_t size);

	static void FreeBlock(BlockId id) throw ();

	template <typename BlockType, typename... Args>
	static BlockId NewBlock(Args... args);

	template <typename BlockType, typename... Args>
	static BlockId NewCustomSizeBlock(size_t size, Args... args);

	template <typename BlockType>
	static void DeleteBlock(BlockId id); // doesn't throw unless ~T() does

	template <typename BlockType>
	static BlockType *BlockPointer(BlockId id);

	template <typename BlockType>
	static BlockType *NonthrowingBlockPointer(BlockId id) throw ();

	template <typename ResourceType, typename... Args>
	static ResourceManager::Allocation<ResourceType> NewResource(Args... args);

	template <typename ResourceType, typename... Args>
	static ResourceType &Resource(ResourceId id);

	static void DeleteResource(ResourceId id) throw ();

	static bool ResourceLost(ResourceId id) throw ();

	static void WaitEvent(int fd, short events);

	static SystemObjectsBlock *SystemObjects();

	static const SystemObjectsBlock *NonthrowingSystemObjects() throw ();

	static Object LoadBuiltinName(const Object &name);

	static Object ImportBuiltinModule(const Object &name);

	Context();

	explicit Context(const Snapshot &snapshot);

	Snapshot snapshot() const throw ();

	void poll_events();

private:
	SystemObjectsBlock *system_objects();

	Arena m_arena;
	ResourceManager m_resource_manager;
	BlockId m_system_objects;
};

typedef ActiveScope<Context> ContextScope;

} // namespace

#include "context-inline.hpp"

#endif
