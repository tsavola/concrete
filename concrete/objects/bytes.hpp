/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_BYTES_HPP
#define CONCRETE_OBJECTS_BYTES_HPP

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <concrete/block.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct BytesBlock: ObjectBlock {
	uint8_t data[0];

	BytesBlock(const TypeObject &type, const uint8_t *data_): ObjectBlock(type)
	{
		std::memcpy(data, data_, size());
	}

	size_t size() const throw ()
	{
		return block_size() - sizeof (BytesBlock);
	}
} CONCRETE_PACKED;

template <typename Ops>
class BytesLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::BuiltinObjects().bytes_type;
	}

	static BytesLogic New(const uint8_t *data, size_t size)
	{
		return Context::NewCustomSizeBlock<BytesBlock>(sizeof (BytesBlock) + size, Type(), data);
	}

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	BytesLogic(const BytesLogic<OtherOps> &other) throw ():
		ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	BytesLogic &operator=(const BytesLogic<OtherOps> &other) throw ()
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	size_t size() const
	{
		return bytes_block()->size();
	}

	const uint8_t *data() const
	{
		return bytes_block()->data;
	}

protected:
	BytesLogic(BlockId id) throw ():
		ObjectLogic<Ops>(id)
	{
	}

	BytesBlock *bytes_block() const
	{
		return static_cast<BytesBlock *> (ObjectLogic<Ops>::object_block());
	}
} CONCRETE_PACKED;

typedef BytesLogic<ObjectOps>         BytesObject;
typedef BytesLogic<PortableObjectOps> PortableBytesObject;

void BytesTypeInit(const TypeObject &type);

} // namespace

#endif
