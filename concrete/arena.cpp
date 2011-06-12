/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

// TODO: proper memory allocator

#include "arena.hpp"

#include <cassert>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <concrete/util/backtrace.hpp>
#include <concrete/util/byteorder.hpp>

namespace concrete {

AllocError::AllocError(size_t size) throw ():
	m_size(size)
{
	Backtrace();
}

AllocError::~AllocError() throw ()
{
}

const char *AllocError::what() const throw ()
{
	return "Block allocation failed";
}

size_t AllocError::size() const throw ()
{
	return m_size;
}

AccessError::AccessError(BlockId block_id, bool deferred) throw ():
	IntegrityError(block_id),
	m_deferred(deferred)
{
}

const char *AccessError::what() const throw ()
{
	if (m_deferred)
		return "Bad block access (deferred)";
	else
		return "Bad block access";
}

Arena::Snapshot::Snapshot() throw ()
{
}

Arena::Snapshot::Snapshot(void *base, size_t size, BlockId access_error_block) throw ():
	base(base),
	size(size),
	access_error_block(access_error_block)
{
}

size_t Arena::Snapshot::head_size() const throw ()
{
	return sizeof (*this) - sizeof (base);
}

const void *Arena::Snapshot::head_ptr() const throw ()
{
	return reinterpret_cast<const char *> (this) + sizeof (base);
}

void *Arena::Snapshot::head_ptr() throw ()
{
	return reinterpret_cast<char *> (this) + sizeof (base);
}

size_t Arena::Snapshot::data_size() const throw ()
{
	return size;
}

const void *Arena::Snapshot::data_ptr() const throw ()
{
	return base;
}

void Arena::Snapshot::init_data(void *data) throw ()
{
	base = data;
}

BlockId Arena::OffsetId(size_t block_offset) throw ()
{
	assert ((block_offset & 1) == 0);

	return block_offset | 1;
}

size_t Arena::IdOffset(BlockId block_id) throw ()
{
	assert ((block_id & 1) == 1);

	return block_id & ~1;
}

size_t Arena::AlignedSize(BlockSize block_size) throw ()
{
	return (block_size + sizeof (uint32_t) - 1) & ~size_t(sizeof (uint32_t) - 1);
}

Arena::Arena() throw ():
	m_base(NULL),
	m_size(0),
	m_access_error_block(0)
{
}

Arena::Arena(const Snapshot &snapshot) throw ():
	m_base(snapshot.base),
	m_size(snapshot.size),
	m_access_error_block(snapshot.access_error_block)
{
}

Arena::~Arena() throw ()
{
	std::free(m_base);
}

Arena::Snapshot Arena::snapshot() const throw ()
{
	return Snapshot(m_base, m_size, m_access_error_block);
}

Arena::Allocation Arena::alloc_block(size_t block_size)
{
	assert(block_size >= sizeof (Block));

	check_error();

	size_t aligned_size = AlignedSize(block_size);
	if (aligned_size < block_size)
		throw AllocError(block_size);

	size_t offset = m_size;

	m_base = std::realloc(m_base, m_size + aligned_size);
	if (m_base == NULL)
		throw AllocError(block_size);

	m_size += aligned_size;

	char *ptr = reinterpret_cast<char *> (m_base) + offset;
	std::memset(ptr, 0, aligned_size);

	Block *block = reinterpret_cast<Block *> (ptr);
	block->m_portable_size = PortByteorder(block_size);

	return Allocation {
		block,
		OffsetId(offset),
	};
}

void Arena::free_block(BlockId block_id) throw ()
{
	auto block = nonthrowing_block_pointer(block_id, sizeof (Block));

	if (block == NULL)
		return;

	size_t offset = IdOffset(block_id);
	size_t aligned_size = AlignedSize(block->block_size());

	if (offset + aligned_size == m_size) {
		m_size -= aligned_size;
		m_base = std::realloc(m_base, m_size);
	} else {
		std::memset(block, 0xff, aligned_size);
	}
}

Block *Arena::block_pointer(BlockId block_id, size_t minimum_size)
{
	check_error();

	auto block = nonthrowing_block_pointer(block_id, minimum_size);

	check_error();

	return block;
}

Block *Arena::nonthrowing_block_pointer(BlockId block_id, size_t minimum_size) throw ()
{
	assert(minimum_size >= sizeof (Block));

	if (block_id == 0)
		return set_access_error(block_id);

	size_t offset = IdOffset(block_id);

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

BlockId Arena::block_id(const Block *block) const throw ()
{
	const char *block_ptr = reinterpret_cast<const char *> (block);
	const char *base_ptr = reinterpret_cast<const char *> (m_base);

	return OffsetId(block_ptr - base_ptr);
}

Block *Arena::set_access_error(BlockId block_id) throw ()
{
	if (m_access_error_block == 0) {
		Backtrace();
		m_access_error_block = block_id;
	}

	return NULL;
}

void Arena::check_error()
{
	if (m_access_error_block) {
		auto block_id = m_access_error_block;
		m_access_error_block = 0;

		throw AccessError(block_id, true);
	}
}

} // namespace
