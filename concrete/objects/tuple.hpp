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

#include <stdexcept>

#include <concrete/block.hpp>
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

	unsigned int size() const
	{
		return (block_size() - sizeof (TupleBlock)) / sizeof (PortableObject);
	}
} CONCRETE_PACKED;

template <typename Ops>
class tuple_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().tuple_type;
	}

	static tuple_object New(unsigned int size) throw (AllocError)
	{
		auto id = Context::Alloc(sizeof (TupleBlock) + sizeof (PortableObject) * size);
		new (Context::Pointer(id)) TupleBlock(Type());
		return id;
	}

	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	tuple_object(const tuple_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	tuple_object &operator=(const tuple_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
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
			throw std::range_error("tuple index out of bounds");
		return block->items[index];
	}

protected:
	tuple_object(BlockId id): object<Ops>(id)
	{
	}

	TupleBlock *tuple_block() const
	{
		return static_cast<TupleBlock *> (object<Ops>::object_block());
	}
} CONCRETE_PACKED;

typedef tuple_object<ObjectOps>         TupleObject;
typedef tuple_object<PortableObjectOps> PortableTupleObject;

} // namespace

#endif
