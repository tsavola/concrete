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

#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

class Arena;

typedef uint32_t BlockSize;
typedef uint32_t BlockId;

class Block {
	friend class Arena;

public:
	Block() throw ();
	Block &operator=(const Block &other) throw ();

	BlockSize block_size() const throw ();

private:
	Block(const Block &);

	BlockSize m_portable_size;

} CONCRETE_PACKED;

class IntegrityError: public std::exception {
public:
	explicit IntegrityError(const Block *block) throw ();
	virtual ~IntegrityError() throw ();

	virtual const char *what() const throw ();
	BlockId block_id() const throw ();

protected:
	explicit IntegrityError(BlockId block_id) throw ();

private:
	const BlockId m_block_id;
};

} // namespace

#endif
