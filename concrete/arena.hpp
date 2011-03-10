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
	Arena() throw (): m_base(NULL), m_size(0)
	{
	}

	Arena(void *base, size_t size) throw (): m_base(base), m_size(size)
	{
	}

	~Arena() throw ();

	BlockId alloc(size_t size);
	void free(Block *block);

	// TODO: verify caller-supplied block type size against actual block size
	Block *pointer(BlockId block_id) const
	{
#ifndef NDEBUG
		if (block_id & (sizeof (uint32_t) - 1))
			throw AccessError(block_id);
#endif

		if (m_size < sizeof (Block) || block_id > m_size - sizeof (Block))
			throw AccessError(block_id);

		auto block = reinterpret_cast<Block *> (reinterpret_cast<char *> (m_base) + block_id);
		auto aligned_size = AlignedSize(block_id, block->block_size());

		if (m_size < aligned_size || block_id > m_size - aligned_size)
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
	static size_t AlignedSize(BlockId block_id, BlockSize block_size)
	{
		size_t aligned_size = UnverifiedAlignedSize(block_size);

		if (aligned_size < block_size)
			throw AccessError(block_id);

		return aligned_size;
	}

	static size_t UnverifiedAlignedSize(BlockSize block_size) throw ()
	{
		return (block_size + sizeof (uint32_t) - 1) & ~size_t(sizeof (uint32_t) - 1);
	}

	void *m_base;
	size_t m_size;
};

} // namespace

#endif
