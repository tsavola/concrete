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
#include <iostream>

#include <boost/format.hpp>

namespace concrete {

#ifdef CONCRETE_BLOCK_MAGIC
static void __attribute__ ((constructor)) block_magic_warning()
{
	std::cerr << "Warning: Block magic enabled\n";
}
#endif

Arena::~Arena() throw ()
{
	std::free(m_base);
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
#ifdef CONCRETE_BLOCK_MAGIC
	block->m_magic = 0xdeadbeef;
#endif
	block->m_size = block_size;

	return Allocation {
		block,
		BlockId::New(offset),
	};
}

void Arena::free_block(BlockId block_id) throw ()
{
	auto block = nonthrowing_block_pointer(block_id, sizeof (Block));

	if (block == NULL)
		return;

	size_t offset = block_id.offset();
	size_t aligned_size = AlignedSize(block->block_size());

	if (offset + aligned_size == m_size) {
		m_size -= aligned_size;
		m_base = std::realloc(m_base, m_size);
	} else {
		std::memset(block, 0xff, aligned_size);
	}
}

void Arena::dump() const
{
	std::cerr << "----- block space dump ------------------------------------------------------\n";

	for (unsigned int i = 0; i < m_size / 4; ) {
		std::cerr << boost::format("%4x:") % (i * 4);

		for (unsigned int j = 0; j < 8 && i < m_size / 4; j++, i++) {
			std::cerr <<
				boost::format(" %08x") % reinterpret_cast<uint32_t *> (m_base)[i];
		}

		std::cerr << "\n";
	}

	std::cerr << "---------------------------------------------------- end of block space -----\n";
}

} // namespace
