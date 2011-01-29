/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_LONG_HPP
#define CONCRETE_OBJECTS_LONG_HPP

#include <cstdint>

#include <concrete/block.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/util/portable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct LongBlock: ObjectBlock {
	portable<int32_t> value;

	LongBlock(const TypeObject &type, int value): ObjectBlock(type), value(value)
	{
	}

} CONCRETE_PACKED;

template <typename Ops>
class long_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().long_type;
	}

	static long_object New(int value) throw (AllocError)
	{
		auto id = Context::Alloc(sizeof (LongBlock));
		new (Context::Pointer(id)) LongBlock(Type(), value);
		return id;
	}

	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	long_object(const long_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	long_object &operator=(const long_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
		return *this;
	}

	int value() const
	{
		return long_block()->value;
	}

protected:
	long_object(BlockId id): object<Ops>(id)
	{
	}

	LongBlock *long_block() const
	{
		return static_cast<LongBlock *> (object<Ops>::object_block());
	}

} CONCRETE_PACKED;

typedef long_object<ObjectOps>         LongObject;
typedef long_object<PortableObjectOps> PortableLongObject;

} // namespace

#endif
