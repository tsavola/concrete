/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

namespace concrete {

template <typename BlockType, typename... Args>
BlockId Arena::new_block(Args... args)
{
	return new_custom_size_block<BlockType>(sizeof (BlockType), args...);
}

template <typename BlockType, typename... Args>
BlockId Arena::new_custom_size_block(size_t size, Args... args)
{
	assert(size >= sizeof (BlockType));

	auto allocation = alloc_block(size);
	new (static_cast<BlockType *> (allocation.ptr)) BlockType(args...);
	return allocation.id;
}

template <typename BlockType>
void Arena::delete_block(BlockId id) // doesn't throw unless ~BlockType() does
{
	auto ptr = nonthrowing_block_pointer<BlockType>(id);
	if (ptr) {
		ptr->~BlockType();
		free_block(id);
	}
}

template <typename BlockType>
BlockType *Arena::block_pointer(BlockId id)
{
	return static_cast<BlockType *> (block_pointer(id, sizeof (BlockType)));
}

template <typename BlockType>
BlockType *Arena::nonthrowing_block_pointer(BlockId id) throw ()
{
	return static_cast<BlockType *> (nonthrowing_block_pointer(id, sizeof (BlockType)));
}

} // namespace
