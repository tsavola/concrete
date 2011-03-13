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
#include <concrete/util/backtrace.hpp>
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

class AccessError: public std::exception {
public:
	explicit AccessError(BlockId block_id) throw (): m_block_id(block_id)
	{
		Backtrace();
	}

	virtual ~AccessError() throw ()
	{
	}

	virtual const char *what() const throw ()
	{
		return "Bad block access";
	}

	BlockId block_id() const throw ()
	{
		return m_block_id;
	}

private:
	const BlockId m_block_id;
};

class Arena: Noncopyable {
public:
	struct Allocation {
		Block *ptr;
		BlockId id;
	};

	Arena() throw (): m_base(NULL), m_size(0)
	{
	}

	Arena(void *base, size_t size) throw (): m_base(base), m_size(size)
	{
	}

	~Arena() throw ();

	Allocation alloc(size_t size);
	void free(BlockId id);

	Block *pointer(BlockId block_id, size_t minimum_size) const
	{
		assert(minimum_size >= sizeof (Block));

		if (block_id == NULL)
			throw AccessError(block_id);

		size_t offset = block_id.offset();

#ifndef NDEBUG
		if (offset & (sizeof (uint32_t) - 1))
			throw AccessError(block_id);
#endif

		if (m_size < minimum_size || offset > m_size - minimum_size)
			throw AccessError(block_id);

		auto block = reinterpret_cast<Block *> (reinterpret_cast<char *> (m_base) + offset);
		size_t block_size = block->block_size();
		size_t aligned_size = AlignedSize(block_size);

		if (aligned_size < block_size)
			throw AccessError(block_id);

		if (m_size < aligned_size || offset > m_size - aligned_size)
			throw AccessError(block_id);

		return block;
	}

	void *base() const throw ()
	{
		return m_base;
	}

	size_t size() const throw ()
	{
		return m_size;
	}

	void dump() const;

private:
	static size_t AlignedSize(BlockSize block_size) throw ()
	{
		return (block_size + sizeof (uint32_t) - 1) & ~size_t(sizeof (uint32_t) - 1);
	}

	void *m_base;
	size_t m_size;
};

} // namespace

#endif
