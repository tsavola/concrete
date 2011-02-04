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

#include <stdexcept>

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

	portable<uint32_t> size;
	Item items[0];

	DictBlock(const TypeObject &type): ObjectBlock(type), size(0)
	{
	}

	~DictBlock()
	{
		for (unsigned int i = size; i-- > 0; )
			items[i].~Item();
	}

	unsigned int capacity() const
	{
		return (block_size() - sizeof (DictBlock)) / sizeof (Item);
	}
} CONCRETE_PACKED;

template <typename Ops>
class dict_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().dict_type;
	}

	static dict_object New(unsigned int capacity = 16) throw (AllocError)
	{
		auto id = Context::Alloc(sizeof (DictBlock) + sizeof (DictBlock::Item) * capacity);
		new (Context::Pointer(id)) DictBlock(Type());
		return id;
	}

	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	dict_object(const dict_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	dict_object &operator=(const dict_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
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
			throw std::runtime_error("dict capacity exceeded");

		new (&block->items[i]) DictBlock::Item(key, value);
		block->size = i + 1;
	}

	Object get_item(const Object &key) const
	{
		if (!key.check<StringObject>())
			throw KeyError(key);

		auto block = dict_block();
		auto i = find_item(block, key);

		if (i == block->size)
			throw KeyError(key);

		return block->items[i].value;
	}

protected:
	dict_object(BlockId id): object<Ops>(id)
	{
	}

	DictBlock *dict_block() const
	{
		return static_cast<DictBlock *> (object<Ops>::object_block());
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
} CONCRETE_PACKED;

typedef dict_object<ObjectOps>         DictObject;
typedef dict_object<PortableObjectOps> PortableDictObject;

} // namespace

#endif
