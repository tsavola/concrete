/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_LOADER_HPP
#define CONCRETE_UTIL_LOADER_HPP

#include <cstdint>

#include <concrete/exception.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

template <typename State>
class loader: noncopyable {
public:
	loader(State &state): m_state(state)
	{
	}

	bool empty() const
	{
		return m_state.position() >= m_state.size();
	}

	template <typename T>
	T load()
	{
		return portable_ops<T, sizeof (T)>::load(*load_raw<T>(1));
	}

	const char *load_chars(size_t count)
	{
		return load_raw<char>(count);
	}

	template <typename T>
	const T *load_raw(size_t count)
	{
		auto pos = m_state.position();

		if (pos + sizeof (T) * count > m_state.size())
			throw RuntimeError("loader ran out of data");

		auto data = m_state.data() + pos;
		m_state.advance(sizeof (T) * count);

		return reinterpret_cast<const T *> (data);
	}

private:
	State &m_state;
};

} // namespace

#endif
