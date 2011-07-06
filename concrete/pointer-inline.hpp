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
	return NewCustomSizePointer<PointerType>(sizeof (typename PointerType::Data), args...);
}

template <typename PointerType, typename... Args>
PointerType Pointer::NewCustomSizePointer(size_t size, Args... args)
{
	assert(size >= sizeof (typename PointerType::Data));

	auto allocated = Arena::Active().allocate(size);
	new (allocated.data) typename PointerType::Data(args...);

	return PointerType(allocated.address);
}

template <typename DataType>
DataType *Pointer::NonthrowingDataCast(unsigned int address) throw ()
{
	if (address == 0) {
		Trace("pointer %1% access (minimum size %2%)", address, sizeof (DataType));
		Arena::Active().defer_access_error(address);
		return NULL;
	}

	return static_cast<DataType *> (
		Arena::Active().nonthrowing_access(address, sizeof (DataType)).data);
}

template <typename PointerType>
void Pointer::DestroyPointer(PointerType &pointer) throw ()
{
	if (pointer.m_address) {
		auto data = NonthrowingDataCast<typename PointerType::Data>(pointer.m_address);
		if (data)
			DestroyData(pointer.m_address, data);

		pointer.reset_address(0);
	}
}

template <typename DataType>
void Pointer::DestroyData(unsigned int address, DataType *data) throw ()
{
	data->~DataType();
	Arena::Active().free(address, data);
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
