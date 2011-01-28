/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_BLOCK_HPP
#define CONCRETE_BLOCK_HPP

#include <cstdint>

#include <concrete/arena-fwd.hpp>
#include <concrete/util/portable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

typedef uint32_t BlockSize;
typedef uint32_t BlockId;

#define NoBlockId  (~BlockId(0))

class Block {
	friend class Arena;

public:
	Block()
	{
	}

	Block &operator=(const Block &other)
	{
		return *this;
	}

	BlockSize block_size() const
	{
		return m_size;
	}

private:
	Block(const Block &);

#ifdef CONCRETE_BLOCK_MAGIC
	portable<uint32_t> m_magic;
#endif
	portable<BlockSize> m_size;

} CONCRETE_PACKED;

} // namespace

#endif
