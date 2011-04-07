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
#include <exception>

#include <concrete/arena-fwd.hpp>
#include <concrete/util/id.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

typedef uint32_t BlockSize;
typedef uint32_t BlockOffset;

class Block {
	friend class Arena;

public:
	Block() throw ()
	{
	}

	Block &operator=(const Block &other) throw ()
	{
		return *this;
	}

	BlockSize block_size() const throw ()
	{
		return m_size;
	}

private:
	Block(const Block &);

#ifdef CONCRETE_BLOCK_MAGIC
	Portable<uint32_t> m_magic;
#endif
	Portable<BlockSize> m_size;

} CONCRETE_PACKED;

typedef IdOps<BlockOffset>         BlockIdOps;
typedef PortableIdOps<BlockOffset> PortableBlockIdOps;

typedef IdLogic<BlockOffset, BlockIdOps>         BlockId;
typedef IdLogic<BlockOffset, PortableBlockIdOps> PortableBlockId;

class IntegrityError: public std::exception {
public:
	explicit IntegrityError(const Block *block) throw ();

	virtual ~IntegrityError() throw ()
	{
	}

	virtual const char *what() const throw ()
	{
		return "Block integrity violation";
	}

	BlockId block_id() const throw ()
	{
		return m_block_id;
	}

protected:
	IntegrityError(BlockId block_id) throw ():
		m_block_id(block_id)
	{
	}

private:
	const BlockId m_block_id;
};

} // namespace

#endif
