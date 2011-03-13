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
#include <concrete/util/activatable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct ContextSnapshot {
	void *base;
	size_t size;
	BlockId builtin_none;
	BlockId builtins;
};

class Context: public Activatable<Context>, Noncopyable {
	struct BuiltinNoneBlock: Block {
		const PortableNoneObject none;

		BuiltinNoneBlock(const NoneObject &none): none(none)
		{
		}
	} CONCRETE_PACKED;

public:
	struct BuiltinsBlock: Block {
		const PortableTypeObject type_type;
		const PortableTypeObject object_type;
		const PortableTypeObject none_type;
		const PortableTypeObject string_type;
		const PortableTypeObject long_type;
		const PortableTypeObject bytes_type;
		const PortableTypeObject tuple_type;
		const PortableTypeObject dict_type;
		const PortableTypeObject code_type;
		const PortableTypeObject function_type;
		const PortableTypeObject internal_type;
		const PortableTypeObject module_type;

		PortableObject modules;

		BuiltinsBlock(const TypeObject &type_type,
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

	template <typename T, typename... Args>
	static BlockId NewBlock(Args... args)
	{
		return Active().new_block<T>(args...);
	}

	template <typename T, typename... Args>
	static BlockId NewCustomSizeBlock(size_t size, Args... args)
	{
		return Active().new_custom_size_block<T>(size, args...);
	}

	template <typename T>
	static void DeleteBlock(BlockId id)
	{
		Active().delete_block<T>(id);
	}

	template <typename T>
	static T *BlockPointer(BlockId id)
	{
		return Active().block_pointer<T>(id);
	}

	static const PortableNoneObject &None()
	{
		return Active().builtin_none().none;
	}

	static const BuiltinsBlock &Builtins()
	{
		return Active().builtins();
	}

	static Object ImportBuiltin(const Object &name);

	Context();

	Context(const ContextSnapshot &snapshot) throw ():
		m_arena(snapshot.base, snapshot.size),
		m_builtin_none(snapshot.builtin_none),
		m_builtins(snapshot.builtins)
	{
	}

	ContextSnapshot snapshot() const throw ()
	{
		return ContextSnapshot {
			m_arena.base(),
			m_arena.size(),
			m_builtin_none,
			m_builtins,
		};
	}

	template <typename T, typename... Args>
	BlockId new_block(Args... args)
	{
		return new_custom_size_block<T>(sizeof (T), args...);
	}

	template <typename T, typename... Args>
	BlockId new_custom_size_block(size_t size, Args... args)
	{
		assert(size >= sizeof (T));

		auto ret = m_arena.alloc(size);
		new (static_cast<T *> (ret.ptr)) T(args...);
		return ret.id;
	}

	template <typename T>
	void delete_block(BlockId id)
	{
		block_pointer<T>(id)->~T();
		m_arena.free(id);
	}

	template <typename T>
	T *block_pointer(BlockId id)
	{
		return static_cast<T *> (m_arena.pointer(id, sizeof (T)));
	}

	Arena &arena()
	{
		return m_arena;
	}

private:
	BuiltinNoneBlock &builtin_none()
	{
		return *block_pointer<BuiltinNoneBlock>(m_builtin_none);
	}

	BuiltinsBlock &builtins()
	{
		return *block_pointer<BuiltinsBlock>(m_builtins);
	}

	Arena m_arena;
	BlockId m_builtin_none;
	BlockId m_builtins;
};

typedef ActiveScope<Context> ContextScope;

} // namespace

#endif
