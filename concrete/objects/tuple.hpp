/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_TUPLE_HPP
#define CONCRETE_OBJECTS_TUPLE_HPP

#include <concrete/block.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct TupleBlock: ObjectBlock {
	PortableObject items[0];

	TupleBlock(const TypeObject &type): ObjectBlock(type)
	{
		for (unsigned int i = 0; i < size(); i++)
			new (&items[i]) PortableObject();
	}

	~TupleBlock()
	{
		for (unsigned int i = size(); i-- > 0; )
			items[i].~PortableObject();
	}

	unsigned int size() const throw ()
	{
		return (block_size() - sizeof (TupleBlock)) / sizeof (PortableObject);
	}
} CONCRETE_PACKED;

template <typename Ops>
class TupleLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().tuple_type;
	}

	static TupleLogic New(unsigned int size)
	{
		return Context::NewCustomSizeBlock<TupleBlock>(
			sizeof (TupleBlock) + sizeof (PortableObject) * size,
			Type());
	}

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	TupleLogic(const TupleLogic<OtherOps> &other): ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	TupleLogic &operator=(const TupleLogic<OtherOps> &other)
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	void init_item(unsigned int index, const Object &item)
	{
		auto block = tuple_block();
		assert(index < block->size());
		block->items[index] = item;
	}

	unsigned int size() const
	{
		return tuple_block()->size();
	}

	Object get_item(unsigned int index) const
	{
		auto block = tuple_block();
		if (index >= block->size())
			throw RuntimeError("tuple index out of bounds");
		return block->items[index];
	}

protected:
	TupleLogic(BlockId id): ObjectLogic<Ops>(id)
	{
	}

	TupleBlock *tuple_block() const
	{
		return static_cast<TupleBlock *> (ObjectLogic<Ops>::object_block());
	}
} CONCRETE_PACKED;

typedef TupleLogic<ObjectOps>         TupleObject;
typedef TupleLogic<PortableObjectOps> PortableTupleObject;

} // namespace

#endif
