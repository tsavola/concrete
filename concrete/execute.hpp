/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_EXECUTE_HPP
#define CONCRETE_EXECUTE_HPP

#include <concrete/block.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/util/activatable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

struct ExecutorSnapshot {
	PortableBlockId initial_frame_id;
	PortableBlockId current_frame_id;

	ExecutorSnapshot() throw ()
	{
	}

	ExecutorSnapshot(BlockId initial_frame_id, BlockId current_frame_id) throw ():
		initial_frame_id(initial_frame_id),
		current_frame_id(current_frame_id)
	{
	}

	size_t size() const throw ()
	{
		return sizeof (*this);
	}

	const void *ptr() const throw ()
	{
		return reinterpret_cast<const void *> (this);
	}

	void *ptr() throw ()
	{
		return reinterpret_cast<void *> (this);
	}
} CONCRETE_PACKED;

class Executor: Activatable<Executor>, Noncopyable {
	friend class Activatable<Executor>;
	friend class ActiveScope<Executor>;

	class Impl;

public:
	static Executor &Active() throw ()
	{
		return Activatable<Executor>::Active();
	}

	explicit Executor(const CodeObject &code);
	explicit Executor(const ExecutorSnapshot &snapshot);
	~Executor() throw ();

	ExecutorSnapshot snapshot() const throw ();

	bool execute();

	BlockId new_frame(const CodeObject &code, const DictObject &dict);
	Object frame_result(BlockId frame);
	void destroy_frame(BlockId frame) throw ();

private:
	Impl *m_impl;
};

} // namespace

#endif
