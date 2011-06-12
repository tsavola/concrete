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
BlockId Context::NewBlock(Args... args)
{
	return Active().m_arena.new_block<BlockType>(args...);
}

template <typename BlockType, typename... Args>
BlockId Context::NewCustomSizeBlock(size_t size, Args... args)
{
	return Active().m_arena.new_custom_size_block<BlockType>(size, args...);
}

template <typename BlockType>
void Context::DeleteBlock(BlockId id) // doesn't throw unless ~T() does
{
	Active().m_arena.delete_block<BlockType>(id);
}

template <typename BlockType>
BlockType *Context::BlockPointer(BlockId id)
{
	return Active().m_arena.block_pointer<BlockType>(id);
}

template <typename BlockType>
BlockType *Context::NonthrowingBlockPointer(BlockId id) throw ()
{
	return Active().m_arena.nonthrowing_block_pointer<BlockType>(id);
}

template <typename ResourceType, typename... Args>
ResourceManager::Allocation<ResourceType> Context::NewResource(Args... args)
{
	return Active().m_resource_manager.new_resource<ResourceType>(args...);
}

template <typename ResourceType, typename... Args>
ResourceType &Context::Resource(ResourceId id)
{
	return Active().m_resource_manager.resource<ResourceType>(id);
}

} // namespace
