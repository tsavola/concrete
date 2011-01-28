/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_ARENA_HPP
#define CONCRETE_ARENA_HPP

#include "arena-fwd.hpp"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>

#include <concrete/block.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

class AllocError: public std::exception {
public:
	explicit AllocError(size_t size) throw (): m_size(size)
	{
	}

	virtual ~AllocError() throw ()
	{
	}

	virtual const char *what() const throw ()
	{
		return "Block allocation failed";
	}

	size_t size() const throw ()
	{
		return m_size;
	}

private:
	const size_t m_size;
};

class Arena: noncopyable {
public:
	Arena(): m_base(NULL), m_size(0)
	{
	}

	Arena(void *base, size_t size): m_base(base), m_size(size)
	{
	}

	~Arena();

	BlockId alloc(size_t size) throw (AllocError);
	void free(Block *block);

	Block *pointer(BlockId id) const
	{
		assert(id < m_size);
		assert((id & (sizeof (uint32_t) - 1)) == 0);
		return reinterpret_cast<Block *> (reinterpret_cast<char *> (m_base) + id);
	}

	void *base() const
	{
		return m_base;
	}

	size_t size() const
	{
		return m_size;
	}

	void dump() const;

private:
	void *m_base;
	size_t m_size;
};

} // namespace

#endif
