/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_BYTEORDER_HPP
#define CONCRETE_UTIL_BYTEORDER_HPP

#ifdef __linux__
# include <endian.h>
#else
# include <sys/types.h>
#endif

#if defined(__BYTE_ORDER)
# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define CONCRETE_LITTLE_ENDIAN
# else
#  define CONCRETE_BIG_ENDIAN
# endif
#elif defined(__APPLE__)
# if defined(__LITTLE_ENDIAN__)
#  define CONCRETE_LITTLE_ENDIAN
# else
#  define CONCRETE_BIG_ENDIAN
# endif
#else
# error cannot figure out byteorder
#endif

namespace concrete {

#ifdef CONCRETE_LITTLE_ENDIAN
template <typename T, unsigned int N>
struct PortableByteorder {
	static T Adapt(const T &x) throw ()
	{
		return x;
	}
};
#else
template <typename T, unsigned int N> struct PortableByteorder;

template <typename T>
struct PortableByteorder<T, 1> {
	static T Adapt(const T &x) throw ()
	{
		return x;
	}
};

template <typename T>
struct PortableByteorder<T, 2> {
	static T Adapt(const T &x) throw ()
	{
		return bswap_16(x);
	}
};

template <typename T>
struct PortableByteorder<T, 4> {
	static T Adapt(const T &x) throw ()
	{
		return bswap_32(x);
	}
};

template <typename T>
struct PortableByteorder<T, 8> {
	static T Adapt(const T &x) throw ()
	{
		return bswap_64(x);
	}
};
#endif

template <typename T>
T PortByteorder(const T &x) throw ()
{
	return PortableByteorder<T, sizeof (T)>::Adapt(x);
}

} // namespace

#endif
