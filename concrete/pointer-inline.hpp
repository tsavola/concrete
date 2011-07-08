/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cassert>

#include <concrete/arena.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

template <typename PointerType>
PointerType Pointer::RawAccess::Materialize(RawType address) throw ()
{
	return PointerType(address);
}

template <typename PointerType>
PointerType Pointer::RawAccess::Materialize(RawType address, PointerType *) throw ()
{
	return PointerType(address);
}

template <typename PointerType, typename... Args>
PointerType Pointer::NewPointer(Args... args)
{
	return PointerType(NewData<typename PointerType::Data>(args...));
}

template <typename PointerType, typename... Args>
PointerType Pointer::NewCustomSizePointer(size_t size, Args... args)
{
	return PointerType(NewCustomSizeData<typename PointerType::Data>(size, args...));
}

template <typename PointerType>
void Pointer::DestroyPointer(PointerType &pointer) throw ()
{
	DestroyData<typename PointerType::Data>(pointer.m_address);
	pointer.reset_address(0);
}

inline Pointer::Pointer(const Pointer &other) throw ():
	ArenaAccess(other),
	m_address(other.m_address)
{
}

inline void Pointer::operator=(const Pointer &other) throw ()
{
	ArenaAccess::operator=(other);
	m_address = other.m_address;
}

template <typename PointerType>
PointerType Pointer::cast() const throw ()
{
	return RawAccess::Materialize<PointerType>(m_address);
}

template <typename DataType>
DataType *Pointer::data_cast() const
{
	return static_cast<DataType *> (arena_access(m_address, sizeof (DataType)));
}

template <typename DataType>
DataType *Pointer::nonthrowing_data_cast() const throw ()
{
	return static_cast<DataType *> (nonthrowing_arena_access(m_address, sizeof (DataType)));
}

} // namespace
