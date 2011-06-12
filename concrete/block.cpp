/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "block.hpp"

#include <concrete/arena.hpp>
#include <concrete/context.hpp>
#include <concrete/util/backtrace.hpp>
#include <concrete/util/byteorder.hpp>

namespace concrete {

Block::Block() throw ()
{
}

Block &Block::operator=(const Block &other) throw ()
{
	return *this;
}

BlockSize Block::block_size() const throw ()
{
	return PortByteorder(m_portable_size);
}

IntegrityError::IntegrityError(BlockId block_id) throw ():
	m_block_id(block_id)
{
}

IntegrityError::IntegrityError(const Block *block) throw ():
	m_block_id(Context::Active().m_arena.block_id(block))
{
	Backtrace();
}

IntegrityError::~IntegrityError() throw ()
{
}

const char *IntegrityError::what() const throw ()
{
	return "Block integrity violation";
}

BlockId IntegrityError::block_id() const throw ()
{
	return m_block_id;
}

} // namespace
