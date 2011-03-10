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

static size_t aligned_size(size_t size)
{
	return (size + sizeof (uint32_t) - 1) & ~size_t(sizeof (uint32_t) - 1);
}

Arena::~Arena()
{
	std::free(m_base);
}

BlockId Arena::alloc(size_t size)
{
	assert(size >= sizeof (Block));

	auto block_size = aligned_size(size);

	m_base = std::realloc(m_base, m_size + block_size);
	if (m_base == NULL)
		throw AllocError(size);

	m_size += block_size;

	char *ptr = reinterpret_cast<char *> (m_base) + m_size - block_size;
	std::memset(ptr, 0, block_size);

	Block *block = reinterpret_cast<Block *> (ptr);
#ifdef CONCRETE_BLOCK_MAGIC
	block->m_magic = 0xdeadbeef;
#endif
	block->m_size = size;

	return ptr - reinterpret_cast<char *> (m_base);
}

void Arena::free(Block *block)
{
	auto offset = reinterpret_cast<char *> (block) - reinterpret_cast<char *> (m_base);
	auto block_size = aligned_size(block->block_size());

	if (offset + block_size == m_size) {
		m_size -= block_size;
		m_base = std::realloc(m_base, m_size);
	} else {
		std::memset(block, 0xff, block_size);
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
