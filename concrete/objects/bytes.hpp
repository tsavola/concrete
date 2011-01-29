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

	size_t size() const
	{
		return block_size() - sizeof (BytesBlock);
	}

} CONCRETE_PACKED;

template <typename Ops>
class bytes_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().bytes_type;
	}

	static bytes_object New(const uint8_t *data, size_t size) throw (AllocError)
	{
		auto id = Context::Alloc(sizeof (BytesBlock) + size);
		new (Context::Pointer(id)) BytesBlock(Type(), data);
		return id;
	}

	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	bytes_object(const bytes_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	bytes_object &operator=(const bytes_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
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
	bytes_object(BlockId id): object<Ops>(id)
	{
	}

	BytesBlock *bytes_block() const
	{
		return static_cast<BytesBlock *> (object<Ops>::object_block());
	}

} CONCRETE_PACKED;

typedef bytes_object<ObjectOps>         BytesObject;
typedef bytes_object<PortableObjectOps> PortableBytesObject;

} // namespace

#endif
