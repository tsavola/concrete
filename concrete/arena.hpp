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
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

class AllocError: public std::exception {
public:
	explicit AllocError(size_t size) throw (): m_size(size)
	{
		Backtrace();
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
	AccessError(BlockId block_id, bool deferred) throw ():
		m_block_id(block_id),
		m_deferred(deferred)
	{
	}

	virtual ~AccessError() throw ()
	{
	}

	virtual const char *what() const throw ()
	{
		if (m_deferred)
			return "Bad block access (deferred)";
		else
			return "Bad block access";
	}

	BlockId block_id() const throw ()
	{
		return m_block_id;
	}

private:
	const BlockId m_block_id;
	const bool m_deferred;
};

struct ArenaSnapshot {
	void *base;
	Portable<BlockSize> size;
	PortableBlockId access_error_block;

	ArenaSnapshot() throw ()
	{
	}

	ArenaSnapshot(void *base, size_t size, BlockId access_error_block) throw ():
		base(base),
		size(size),
		access_error_block(access_error_block)
	{
	}

	size_t head_size() const throw ()
	{
		return sizeof (*this) - sizeof (base);
	}

	const void *head_ptr() const throw ()
	{
		return reinterpret_cast<const char *> (this) + sizeof (base);
	}

	void *head_ptr() throw ()
	{
		return reinterpret_cast<char *> (this) + sizeof (base);
	}

	size_t data_size() const throw ()
	{
		return size;
	}

	const void *data_ptr() const throw ()
	{
		return base;
	}

	void init_data(void *data) throw ()
	{
		base = data;
	}
} CONCRETE_PACKED;

class Arena: Noncopyable {
public:
	struct Allocation {
		Block *ptr;
		BlockId id;
	};

	Arena() throw ():
		m_base(NULL),
		m_size(0)
	{
	}

	Arena(const ArenaSnapshot &snapshot) throw ():
		m_base(snapshot.base),
		m_size(snapshot.size),
		m_access_error_block(snapshot.access_error_block)
	{
	}

	~Arena() throw ();

	Allocation alloc(size_t size);
	void free(BlockId id) throw ();

	Block *pointer(BlockId block_id, size_t minimum_size)
	{
		check_error();

		auto block = nonthrowing_pointer(block_id, minimum_size);

		check_error();

		return block;
	}

	Block *nonthrowing_pointer(BlockId block_id, size_t minimum_size) throw ()
	{
		assert(minimum_size >= sizeof (Block));

		if (block_id == NULL)
			return set_access_error(block_id);

		size_t offset = block_id.offset();

#ifndef NDEBUG
		if (offset & (sizeof (uint32_t) - 1))
			return set_access_error(block_id);
#endif

		if (m_size < minimum_size || offset > m_size - minimum_size)
			return set_access_error(block_id);

		auto block = reinterpret_cast<Block *> (reinterpret_cast<char *> (m_base) + offset);
		size_t block_size = block->block_size();
		size_t aligned_size = AlignedSize(block_size);

		if (aligned_size < block_size)
			return set_access_error(block_id);

		if (m_size < aligned_size || offset > m_size - aligned_size)
			return set_access_error(block_id);

		return block;
	}

	void check_error()
	{
		if (m_access_error_block) {
			auto block_id = m_access_error_block;
			m_access_error_block = NULL;

			throw AccessError(block_id, true);
		}
	}

	ArenaSnapshot snapshot() const throw ()
	{
		return ArenaSnapshot(m_base, m_size, m_access_error_block);
	}

	void dump() const;

private:
	static size_t AlignedSize(BlockSize block_size) throw ()
	{
		return (block_size + sizeof (uint32_t) - 1) & ~size_t(sizeof (uint32_t) - 1);
	}

	Block *set_access_error(BlockId block_id) throw ()
	{
		if (m_access_error_block == NULL) {
			Backtrace();
			m_access_error_block = block_id;
		}

		return NULL;
	}

	void *m_base;
	size_t m_size;
	BlockId m_access_error_block;
};

} // namespace

#endif
