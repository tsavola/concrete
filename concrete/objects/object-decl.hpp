/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_OBJECT_DECL_HPP
#define CONCRETE_OBJECTS_OBJECT_DECL_HPP

#include "object-fwd.hpp"

#include <stdexcept>

#include <concrete/arena.hpp>
#include <concrete/block.hpp>
#include <concrete/objects/string-fwd.hpp>
#include <concrete/objects/type-fwd.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct ObjectBlock;

template <typename Ops>
class object {
public:
	static TypeObject Type();
	static object New() throw (AllocError);

	object();

	explicit object(BlockId id): m_raw_id(Ops::store(id))
	{
		ref();
	}

	object(const object &other): m_raw_id(other.m_raw_id)
	{
		ref();
	}

	template <typename OtherOps>
	object(const object<OtherOps> &other): m_raw_id(Ops::store(other.id()))
	{
		ref();
	}

	~object()
	{
		unref();
	}

	object &operator=(const object &other)
	{
		unref();
		m_raw_id = other.m_raw_id;
		ref();
		return *this;
	}

	template <typename OtherOps>
	object &operator=(const object<OtherOps> &other)
	{
		unref();
		m_raw_id = Ops::store(other.id());
		ref();
		return *this;
	}

	bool operator==(const object &other) const
	{
		return m_raw_id == other.m_raw_id;
	}

	template <typename OtherOps>
	bool operator==(const object<OtherOps> &other) const
	{
		return id() == other.id();
	}

	bool operator!=(const object &other) const
	{
		return m_raw_id != other.m_raw_id;
	}

	template <typename OtherOps>
	bool operator!=(const object<OtherOps> &other) const
	{
		return id() != other.id();
	}

	template <typename T> bool check() const;
	template <typename T> T cast() const;
	template <typename T> T require() const;

	BlockId id() const
	{
		return Ops::load(m_raw_id);
	}

	TypeObject type() const;
	StringObject repr() const;

protected:
	ObjectBlock *object_block() const;

private:
	void ref() const;
	void unref() const;

	BlockId m_raw_id;

} CONCRETE_PACKED;

struct ObjectProtocol {
	PortableObject repr;
	PortableObject add;
} CONCRETE_PACKED;

} // namespace

#endif
