/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_POINTER_HPP
#define CONCRETE_POINTER_HPP

#include <cstddef>
#include <cstdint>
#include <exception>

#include <concrete/arena.hpp>

namespace concrete {

class NoneObject;
class Object;
class TypeObject;

class Pointer: public ArenaAccess {
	friend class NoneObject;
	friend class Object;
	friend class TypeObject;

public:
	struct RawAccess {
		typedef uint32_t RawType;

		static RawType InitRef() throw () { return 0; }
		static void Ref(RawType address) throw () {}
		static void Unref(RawType address) throw () {}
		static RawType Extract(const Pointer &pointer) throw () { return pointer.m_address; }
		static RawType ExtractRef(const Pointer &pointer) throw () { return pointer.m_address; }

		template <typename PointerType>
		static PointerType Materialize(RawType address) throw ();

		template <typename PointerType>
		static PointerType Materialize(RawType address, PointerType *) throw ();
	};

	Pointer() throw (): m_address(0) {}
	Pointer(const Pointer &other) throw ();

	void operator=(const Pointer &other) throw ();

	bool operator==(const Pointer &other) const throw () { return m_address == other.m_address; }
	bool operator!=(const Pointer &other) const throw () { return m_address != other.m_address; }

	operator bool() const throw () { return m_address != 0; }
	bool operator!() const throw () { return m_address == 0; }

	template <typename PointerType> PointerType cast() const throw ();

	unsigned int address() const throw () { return m_address; }

protected:
	template <typename PointerType, typename... Args>
	static PointerType NewPointer(Args... args);

	template <typename PointerType, typename... Args>
	static PointerType NewCustomSizePointer(size_t size, Args... args);

	template <typename PointerType>
	static void DestroyPointer(PointerType &pointer) throw ();

	explicit Pointer(unsigned int address) throw (): m_address(address) {}

	void reset_address(unsigned int address) throw () { m_address = address; }

	template <typename DataType> DataType *data_cast() const;
	template <typename DataType> DataType *nonthrowing_data_cast() const throw ();

private:
	unsigned int m_address;
};

} // namespace

#define CONCRETE_POINTER_DECL_COMMON(Class, ParentClass)                      \
	protected: friend class concrete::Pointer;                            \
	           explicit Class(unsigned int address) throw ():             \
	                   ParentClass(address) {}                            \
	public:    Class(const Class &other) throw (): ParentClass(other) {}

#define CONCRETE_POINTER_DECL_CONSTRUCT(Class)                                \
	public:    Class() throw () {}

#define CONCRETE_POINTER_DECL_DESTROY(Class)                                  \
	public:    void destroy() throw ();

#define CONCRETE_POINTER_DECL_DATA(Class)                                     \
	protected: struct Data;                                               \
	private:   Data *data() const;                                        \
	public:

#define CONCRETE_POINTER_IMPL_DATA(Class)                                     \
	Class::Data *Class::data() const { return data_cast<Data>(); }

#define CONCRETE_POINTER_IMPL_DESTROY(Class)                                  \
	void Class::destroy() throw () { DestroyPointer(*this); }

#define CONCRETE_POINTER_DEFAULT_DECL(Class, ParentClass)                     \
	CONCRETE_POINTER_DECL_COMMON(Class, ParentClass)                      \
	CONCRETE_POINTER_DECL_CONSTRUCT(Class)                                \
	CONCRETE_POINTER_DECL_DESTROY(Class)                                  \
	CONCRETE_POINTER_DECL_DATA(Class)

#define CONCRETE_POINTER_DEFAULT_IMPL(Class)                                  \
	CONCRETE_POINTER_IMPL_DESTROY(Class)                                  \
	CONCRETE_POINTER_IMPL_DATA(Class)

#include "pointer-inline.hpp"

#endif
