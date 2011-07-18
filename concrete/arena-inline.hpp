/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <concrete/util/assert.hpp>

namespace concrete {

template <typename T, typename... Args>
unsigned int ArenaAccess::NewData(Args... args)
{
	return NewCustomSizeData<T>(sizeof (T), args...);
}

template <typename T, typename... Args>
unsigned int ArenaAccess::NewCustomSizeData(size_t size, Args... args)
{
	assert(size >= sizeof (T));

	auto allocated = Arena::Active().allocate(size);
	new (allocated.data) T(args...);

	return allocated.address;
}

template <typename T>
void ArenaAccess::DestroyData(unsigned int address) throw ()
{
	if (address) {
		auto data = NonthrowingDataCast<T>(address);
		if (data)
			DestroyData(address, data);
	}
}

template <typename T>
void ArenaAccess::DestroyData(unsigned int address, T *data) throw ()
{
	data->~T();
	Arena::Active().free(address, data);
}

template <typename T>
T *ArenaAccess::DataCast(unsigned int address)
{
	return static_cast<T *> (Arena::Active().access(address, sizeof (T)).data);
}

template <typename T>
T *ArenaAccess::NonthrowingDataCast(unsigned int address) throw ()
{
	return static_cast<T *> (Arena::Active().nonthrowing_access(address, sizeof (T)).data);
}

} // namespace
