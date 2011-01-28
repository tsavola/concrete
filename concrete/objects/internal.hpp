/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_INTERNAL_HPP
#define CONCRETE_OBJECTS_INTERNAL_HPP

#include <cstdint>

#include <concrete/block.hpp>
#include <concrete/context.hpp>
#include <concrete/internals.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/portable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

typedef internals::Serial InternalSerial;
typedef Object (*InternalFunction)(const TupleObject &args, const DictObject &kwargs);

struct InternalBlock: ObjectBlock {
	static BlockId New(InternalSerial serial, InternalFunction function) throw (AllocError);

	const portable<uint16_t> serial;

	InternalBlock(const TypeObject &type, InternalSerial serial): ObjectBlock(type), serial(serial)
	{
	}

	Object call(const TupleObject &args, const DictObject &kwargs);

} CONCRETE_PACKED;

template <typename Ops>
class internal_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().internal_type;
	}

	static internal_object New(InternalSerial serial, InternalFunction function) throw (AllocError)
	{
		return InternalBlock::New(serial, function);
	}

	using object<Ops>::operator==;
	using object<Ops>::operator!=;
	using object<Ops>::id;

	template <typename OtherOps>
	internal_object(const internal_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	internal_object &operator=(const internal_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
		return *this;
	}

	Object call(const TupleObject &args, const DictObject &kwargs) const
	{
		return internal_block()->call(args, kwargs);
	}

protected:
	internal_object(BlockId id): object<Ops>(id)
	{
	}

	InternalBlock *internal_block() const
	{
		return static_cast<InternalBlock *> (object<Ops>::object_block());
	}

} CONCRETE_PACKED;

typedef internal_object<ObjectOps>         InternalObject;
typedef internal_object<PortableObjectOps> PortableInternalObject;

} // namespace

#endif
