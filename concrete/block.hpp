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

#include <cassert>
#include <cstddef>
#include <cstdint>

#include <concrete/arena-fwd.hpp>
#include <concrete/util/portable.hpp>
#include <concrete/util/packed.hpp>

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

struct BlockIdOps {
	static const BlockOffset &Load(const BlockOffset &offset) throw ()
	{
		return offset;
	}

	static const BlockOffset &Store(const BlockOffset &offset) throw ()
	{
		return offset;
	}
};

struct PortableBlockIdOps {
	static BlockOffset Load(BlockOffset offset) throw ()
	{
		return PortableOps<BlockOffset, sizeof (offset)>::Load(offset);
	}

	static BlockOffset Store(BlockOffset offset) throw ()
	{
		return PortableOps<BlockOffset, sizeof (offset)>::Store(offset);
	}
};

template <typename Ops>
class BlockIdLogic {
	friend class Arena;

	// use nullptr_t in the future
	typedef decltype (NULL) Null;

public:
	BlockIdLogic() throw ():
		m_raw_offset(NoRawOffset())
	{
	}

	BlockIdLogic(const BlockIdLogic &other) throw ():
		m_raw_offset(other.m_raw_offset)
	{
	}

	template <typename OtherOps>
	BlockIdLogic(const BlockIdLogic<OtherOps> &other) throw ():
		m_raw_offset(Ops::Store(other.offset()))
	{
	}

	BlockIdLogic(const Null &null) throw ():
		m_raw_offset(NoRawOffset())
	{
		assert(!null);
	}

	BlockIdLogic &operator=(const BlockIdLogic &other) throw ()
	{
		m_raw_offset = other.m_raw_offset;
		return *this;
	}

	template <typename OtherOps>
	BlockIdLogic &operator=(const BlockIdLogic<OtherOps> &other) throw ()
	{
		m_raw_offset = Ops::Store(other.offset());
		return *this;
	}

	BlockIdLogic &operator=(const Null &null) throw ()
	{
		assert(!null);

		m_raw_offset = NoRawOffset();
		return *this;
	}

	bool operator==(const BlockIdLogic &other) const throw ()
	{
		return m_raw_offset == other.m_raw_offset;
	}

	bool operator!=(const BlockIdLogic &other) const throw ()
	{
		return m_raw_offset != other.m_raw_offset;
	}

	template <typename OtherOps>
	bool operator==(const BlockIdLogic<OtherOps> &other) const throw ()
	{
		return offset() == other.offset();
	}

	template <typename OtherOps>
	bool operator!=(const BlockIdLogic<OtherOps> &other) const throw ()
	{
		return offset() != other.offset();
	}

	bool operator==(const Null &null) const throw ()
	{
		assert(!null);

		return m_raw_offset == NoRawOffset();
	}

	bool operator!=(const Null &null) const throw ()
	{
		assert(!null);

		return m_raw_offset != NoRawOffset();
	}

	operator bool() const throw ()
	{
		return m_raw_offset != NoRawOffset();
	}

	BlockOffset offset() const throw ()
	{
		return Ops::Load(m_raw_offset);
	}

private:
	static BlockOffset NoRawOffset() throw ()
	{
		return Ops::Store(~BlockOffset(0));
	}

	static BlockIdLogic New(BlockOffset offset) throw ()
	{
		assert(offset != NoRawOffset());

		BlockIdLogic id;
		id.m_raw_offset = Ops::Store(offset);
		return id;
	}

	BlockOffset m_raw_offset;

} CONCRETE_PACKED;

typedef BlockIdLogic<BlockIdOps>         BlockId;
typedef BlockIdLogic<PortableBlockIdOps> PortableBlockId;

} // namespace

#endif
