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

#include <tuple>

#include <concrete/arena.hpp>
#include <concrete/block.hpp>
#include <concrete/objects/object-decl.hpp>
#include <concrete/objects/type-decl.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/thread.hpp>

namespace concrete {

class Context: noncopyable {
public:
	struct BuiltinsBlock: Block {
		PortableObject none;
		PortableTypeObject type_type;
		PortableTypeObject object_type;
		PortableTypeObject none_type;
		PortableTypeObject string_type;
		PortableTypeObject long_type;
		PortableTypeObject bytes_type;
		PortableTypeObject tuple_type;
		PortableTypeObject dict_type;
		PortableTypeObject code_type;
		PortableTypeObject function_type;
		PortableTypeObject internal_type;
		PortableTypeObject module_type;

		BuiltinsBlock(const Object &none,
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
		              const TypeObject &module_type):
			none(none),
			type_type(type_type),
			object_type(object_type),
			none_type(none_type),
			string_type(string_type),
			long_type(long_type),
			bytes_type(bytes_type),
			tuple_type(tuple_type),
			dict_type(dict_type),
			code_type(code_type),
			function_type(function_type),
			internal_type(internal_type),
			module_type(module_type)
		{
		}

	} CONCRETE_PACKED;

	struct Snapshot {
		void *base;
		size_t size;
		BlockId builtins;
	};

	static BlockId Alloc(size_t size)
	{
		return Active().m_arena.alloc(size);
	}

	static void Free(Block *block)
	{
		return Active().m_arena.free(block);
	}

	template <typename T>
	static BlockId New() throw (AllocError)
	{
		auto id = Alloc(sizeof (T));
		new (Pointer(id)) T();
		return id;
	}

	template <typename T>
	static void Delete(BlockId id)
	{
		static_cast<T *> (Pointer(id))->~T();
		Free(Pointer(id));
	}

	static Block *Pointer(BlockId id)
	{
		return Active().m_arena.pointer(id);
	}

	static const BuiltinsBlock &Builtins()
	{
		return *static_cast<BuiltinsBlock *> (Pointer(Active().m_builtins));
	}

	Context() throw (AllocError);

	Context(const Snapshot &snapshot):
		m_arena(snapshot.base, snapshot.size), m_builtins(snapshot.builtins)
	{
	}

	void activate()
	{
		assert(m_active == NULL);
		m_active = this;
	}

	void deactivate()
	{
		assert(m_active == this);
		m_active = NULL;
	}

	Snapshot snapshot() const
	{
		return Snapshot { m_arena.base(), m_arena.size(), m_builtins };
	}

private:
	static Context &Active()
	{
		assert(m_active);
		return *m_active;
	}

	static CONCRETE_THREAD_LOCAL(Context *) m_active;

	Arena m_arena;
	BlockId m_builtins;
};

class ContextScope: noncopyable {
public:
	explicit ContextScope(Context &context): m_context(context)
	{
		m_context.activate();
	}

	~ContextScope()
	{
		m_context.deactivate();
	}

private:
	Context &m_context;
};

} // namespace

#endif
