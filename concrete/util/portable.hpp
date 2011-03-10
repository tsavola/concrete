/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_PORTABLE_HPP
#define CONCRETE_UTIL_PORTABLE_HPP

#include <concrete/util/byteorder.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

#ifdef CONCRETE_LITTLE_ENDIAN
template <typename T, unsigned int N>
struct PortableOps {
	static inline T Load(const T &x) throw ()  { return x; }
	static inline T Store(const T &x) throw () { return x; }
};
#else
template <typename T, unsigned int N>
struct PortableOps {
	static inline T Load(const T &x) throw ()  { return Byteorder<T, N>::Swap(x); }
	static inline T Store(const T &x) throw () { return Byteorder<T, N>::Swap(x); }
};
#endif

template <typename T>
class Portable {
	typedef PortableOps<T, sizeof (T)> Ops;

public:
	Portable() throw ()
	{
	}

	Portable(const T &x) throw (): m_raw(Ops::Store(x))
	{
	}

	Portable(const Portable &other) throw (): m_raw(other.m_raw)
	{
	}

	Portable &operator=(const T &x) throw ()
	{
		m_raw = Ops::Store(x);
		return *this;
	}

	Portable &operator=(const Portable &other) throw ()
	{
		m_raw = other.m_raw;
		return *this;
	}

	operator T() const throw ()
	{
		return Ops::Load(m_raw);
	}

	bool operator==(const T &x) const throw ()
	{
		return Ops::Load(m_raw) == x;
	}

	bool operator!=(const T &x) const throw ()
	{
		return Ops::Load(m_raw) != x;
	}

	bool operator==(const Portable &other) const throw ()
	{
		return m_raw == other.m_raw;
	}

	bool operator!=(const Portable &other) const throw ()
	{
		return m_raw != other.m_raw;
	}

private:
	T m_raw;

} CONCRETE_PACKED;

} // namespace

#endif
