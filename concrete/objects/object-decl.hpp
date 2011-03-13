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

struct ObjectProtocol;
struct ObjectBlock;

template <typename Ops>
class ObjectLogic {
public:
	static TypeObject Type();
	static ObjectLogic New();

	ObjectLogic();

	explicit ObjectLogic(BlockId id): m_raw_id(Ops::Store(id))
	{
		ref();
	}

	ObjectLogic(const ObjectLogic &other): m_raw_id(other.m_raw_id)
	{
		ref();
	}

	template <typename OtherOps>
	ObjectLogic(const ObjectLogic<OtherOps> &other): m_raw_id(Ops::Store(other.id()))
	{
		ref();
	}

	~ObjectLogic()
	{
		unref();
	}

	ObjectLogic &operator=(const ObjectLogic &other)
	{
		unref();
		m_raw_id = other.m_raw_id;
		ref();
		return *this;
	}

	template <typename OtherOps>
	ObjectLogic &operator=(const ObjectLogic<OtherOps> &other)
	{
		unref();
		m_raw_id = Ops::Store(other.id());
		ref();
		return *this;
	}

	bool operator==(const ObjectLogic &other) const throw ()
	{
		return m_raw_id == other.m_raw_id;
	}

	template <typename OtherOps>
	bool operator==(const ObjectLogic<OtherOps> &other) const throw ()
	{
		return id() == other.id();
	}

	bool operator!=(const ObjectLogic &other) const throw ()
	{
		return m_raw_id != other.m_raw_id;
	}

	template <typename OtherOps>
	bool operator!=(const ObjectLogic<OtherOps> &other) const throw ()
	{
		return id() != other.id();
	}

	template <typename T> bool check() const;
	template <typename T> T cast() const;
	template <typename T> T require() const;

	BlockId id() const throw ()
	{
		return Ops::Load(m_raw_id);
	}

	TypeObject type() const;
	const ObjectProtocol &protocol() const;

	Object add(const Object &) const;
	StringObject repr() const;
	StringObject str() const;

protected:
	ObjectBlock *object_block() const;

private:
	void ref() const;
	void unref() const;

	BlockId m_raw_id;

} CONCRETE_PACKED;

} // namespace

#endif
