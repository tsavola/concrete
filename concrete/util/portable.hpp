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

#include <cstddef>

#include <endian.h>

#include <concrete/util/packed.hpp>

namespace concrete {

template <typename T, size_t N> struct portable_ops;

template <typename T>
struct portable_ops<T, 1> {
	static T load(const T &x)  { return x; }
	static T store(const T &x) { return x; }
};

template <typename T>
struct portable_ops<T, 2> {
	static T load(const T &x)  { return le16toh(x); }
	static T store(const T &x) { return htole16(x); }
};

template <typename T>
struct portable_ops<T, 4> {
	static T load(const T &x)  { return le32toh(x); }
	static T store(const T &x) { return htole32(x); }
};

template <typename T>
struct portable_ops<T, 8> {
	static T load(const T &x)  { return le64toh(x); }
	static T store(const T &x) { return htole64(x); }
};

template <typename T>
class portable {
	typedef portable_ops<T, sizeof (T)> Ops;

public:
	portable()
	{
	}

	portable(const T &x): raw(Ops::store(x))
	{
	}

	portable(const portable &other): raw(other.raw)
	{
	}

	portable &operator=(const T &x)
	{
		raw = Ops::store(x);
		return *this;
	}

	portable &operator=(const portable &other)
	{
		raw = other.raw;
		return *this;
	}

	operator T() const
	{
		return Ops::load(raw);
	}

	bool operator==(const T &x) const
	{
		return Ops::load(raw) == x;
	}

	bool operator!=(const T &x) const
	{
		return Ops::load(raw) != x;
	}

	bool operator==(const portable &other) const
	{
		return raw == other.raw;
	}

	bool operator!=(const portable &other) const
	{
		return raw != other.raw;
	}

private:
	T raw;

} CONCRETE_PACKED;

} // namespace

#endif
