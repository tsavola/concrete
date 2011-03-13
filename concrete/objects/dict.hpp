/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_DICT_HPP
#define CONCRETE_OBJECTS_DICT_HPP

#include <concrete/block.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct DictBlock: ObjectBlock {
	struct Item {
		PortableObject key;
		PortableObject value;

		Item(const Object &key, const Object &value): key(key), value(value)
		{
		}
	} CONCRETE_PACKED;

	Portable<uint32_t> size;
	Item items[0];

	DictBlock(const TypeObject &type): ObjectBlock(type), size(0)
	{
	}

	~DictBlock()
	{
		for (unsigned int i = size; i-- > 0; )
			items[i].~Item();
	}

	unsigned int capacity() const throw ()
	{
		return (block_size() - sizeof (DictBlock)) / sizeof (Item);
	}
} CONCRETE_PACKED;

template <typename Ops>
class DictLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::BuiltinObjects().dict_type;
	}

	static DictLogic EmptySingleton()
	{
		auto empty = Context::BuiltinObjects().dict_empty.cast<DictLogic>();
		assert(empty.capacity() == 0);
		return empty;
	}

	static DictLogic New()
	{
		return NewWithCapacity(16);
	}

	static DictLogic NewWithCapacity(unsigned int capacity)
	{
		return Context::NewCustomSizeBlock<DictBlock>(
			sizeof (DictBlock) + sizeof (DictBlock::Item) * capacity,
			Type());
	}

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	DictLogic(const DictLogic<OtherOps> &other): ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	DictLogic &operator=(const DictLogic<OtherOps> &other)
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	unsigned int size() const
	{
		return dict_block()->size;
	}

	void set_item(const Object &key, const Object &value) const
	{
		key.require<StringObject>();

		auto block = dict_block();
		auto i = find_item(block, key);

		if (i < block->size) {
			block->items[i].value = value;
			return;
		}

		if (i == block->capacity())
			throw RuntimeError("dict capacity exceeded");

		new (&block->items[i]) DictBlock::Item(key, value);
		block->size = i + 1;
	}

	Object get_item(const Object &key) const
	{
		Object value;

		if (!get_item(key, value))
			throw KeyError(key);

		return value;
	}

	bool get_item(const Object &key, Object &value) const
	{
		if (!key.check<StringObject>())
			return false;

		auto block = dict_block();
		auto i = find_item(block, key);

		if (i == block->size)
			return false;

		value = block->items[i].value;
		return true;
	}

	void copy_to(const DictLogic &target) const
	{
		for (unsigned int i = 0; i < size(); i++) {
			auto block = dict_block();
			target.set_item(block->items[i].key, block->items[i].value);
		}
	}

protected:
	DictLogic(BlockId id): ObjectLogic<Ops>(id)
	{
	}

	DictBlock *dict_block() const
	{
		return static_cast<DictBlock *> (ObjectLogic<Ops>::object_block());
	}

private:
	static unsigned int find_item(DictBlock *block, const Object &key)
	{
		unsigned int i;

		for (i = 0; i < block->size; i++)
			if (key.cast<StringObject>().equals(block->items[i].key.cast<StringObject>()))
				break;

		return i;
	}

	unsigned int capacity() const
	{
		return dict_block()->capacity();
	}
} CONCRETE_PACKED;

typedef DictLogic<ObjectOps>         DictObject;
typedef DictLogic<PortableObjectOps> PortableDictObject;

void DictInit(const TypeObject &type);

} // namespace

#endif
