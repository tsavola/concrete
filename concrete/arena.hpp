/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_ARENA_HPP
#define CONCRETE_ARENA_HPP

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <exception>

#include <concrete/block.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

class AllocError: public std::exception {
public:
	explicit AllocError(size_t size) throw ();
	virtual ~AllocError() throw ();

	virtual const char *what() const throw ();
	size_t size() const throw ();

private:
	const size_t m_size;
};

class AccessError: public IntegrityError {
public:
	AccessError(BlockId block_id, bool deferred) throw ();

	virtual const char *what() const throw ();

private:
	const bool m_deferred;
};

class Arena: Noncopyable {
	friend class IntegrityError;

public:
	struct Snapshot {
		void *base;
		Portable<BlockSize> size;
		Portable<BlockId> access_error_block;

		Snapshot() throw ();
		Snapshot(void *base, size_t size, BlockId access_error_block) throw ();

		size_t head_size() const throw ();
		const void *head_ptr() const throw ();
		void *head_ptr() throw ();

		size_t data_size() const throw ();
		const void *data_ptr() const throw ();
		void init_data(void *data) throw ();

	} CONCRETE_PACKED;

	struct Allocation {
		Block *ptr;
		BlockId id;
	};

	Arena() throw ();
	explicit Arena(const Snapshot &snapshot) throw ();
	~Arena() throw ();

	Snapshot snapshot() const throw ();

	Allocation alloc_block(size_t size);
	void free_block(BlockId id) throw ();

	Block *block_pointer(BlockId block_id, size_t minimum_size);
	Block *nonthrowing_block_pointer(BlockId block_id, size_t minimum_size) throw ();

	template <typename BlockType, typename... Args>
	BlockId new_block(Args... args);

	template <typename BlockType, typename... Args>
	BlockId new_custom_size_block(size_t size, Args... args);

	template <typename BlockType>
	void delete_block(BlockId id); // doesn't throw unless ~BlockType() does

	template <typename BlockType>
	BlockType *block_pointer(BlockId id);

	template <typename BlockType>
	BlockType *nonthrowing_block_pointer(BlockId id) throw ();

private:
	static BlockId OffsetId(size_t block_offset) throw ();
	static size_t IdOffset(BlockId block_id) throw ();
	static size_t AlignedSize(BlockSize block_size) throw ();

	BlockId block_id(const Block *block) const throw ();

	Block *set_access_error(BlockId block_id) throw ();
	void check_error();

	void *m_base;
	size_t m_size;
	BlockId m_access_error_block;
};

} // namespace

#include "arena-inline.hpp"

#endif
