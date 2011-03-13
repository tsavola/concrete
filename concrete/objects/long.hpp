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
	Portable<int64_t> value;

	LongBlock(const TypeObject &type, int64_t value): ObjectBlock(type), value(value)
	{
	}
} CONCRETE_PACKED;

template <typename Ops>
class LongLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().long_type;
	}

	static LongLogic New(int64_t value)
	{
		return Context::NewBlock<LongBlock>(Type(), value);
	}

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	LongLogic(const LongLogic<OtherOps> &other): ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	LongLogic &operator=(const LongLogic<OtherOps> &other)
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	int64_t value() const
	{
		return long_block()->value;
	}

protected:
	LongLogic(BlockId id): ObjectLogic<Ops>(id)
	{
	}

	LongBlock *long_block() const
	{
		return static_cast<LongBlock *> (ObjectLogic<Ops>::object_block());
	}
} CONCRETE_PACKED;

typedef LongLogic<ObjectOps>         LongObject;
typedef LongLogic<PortableObjectOps> PortableLongObject;

void LongObjectInit(const TypeObject &type);

} // namespace

#endif
