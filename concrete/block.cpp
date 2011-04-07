/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "block.hpp"

#include <concrete/context.hpp>

namespace concrete {

IntegrityError::IntegrityError(const Block *block) throw ():
	m_block_id(BlockId::New(reinterpret_cast<const char *> (block) -
	                        reinterpret_cast<const char *> (Context::Active().m_arena.m_base)))
{
	Backtrace();
}

#ifdef CONCRETE_BLOCK_MAGIC
static void __attribute__ ((constructor)) block_magic_warning()
{
	std::cerr << "Warning: Block magic enabled\n";
}
#endif

} // namespace
