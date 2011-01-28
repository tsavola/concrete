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
#include <stdexcept>

#include <concrete/util/noncopyable.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

class Loader: noncopyable {
public:
	Loader(const uint8_t *data = NULL, size_t size = 0): m_data(data), m_size(size), m_position(0)
	{
	}

	void reset(const uint8_t *data, size_t size, size_t position = 0)
	{
		m_data = data;
		m_size = size;
		m_position = position;
	}

	size_t position() const
	{
		return m_position;
	}

	bool empty() const
	{
		return m_position >= m_size;
	}

	template <typename T> T load()
	{
		return portable_ops<T, sizeof (T)>::load(*load_raw<T>(1));
	}

	const char *load_chars(size_t count)
	{
		return load_raw<char>(count);
	}

	template <typename T> const T *load_raw(size_t count)
	{
		if (m_position + sizeof (T) * count > m_size)
			throw std::runtime_error("loader ran out of data");

		auto data = m_data + m_position;
		m_position += sizeof (T) * count;

		return reinterpret_cast<const T *> (data);
	}

private:
	const uint8_t *m_data;
	size_t m_size;
	size_t m_position;
};

} // namespace

#endif
