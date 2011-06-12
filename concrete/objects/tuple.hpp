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

#include <concrete/objects/object.hpp>

namespace concrete {

class TupleObject: public Object {
	friend class Object;

public:
	static TypeObject Type();

	static TupleObject New();
	template <typename Item, typename... Tail> static TupleObject New(Item, Tail...);
	static TupleObject NewWithSize(unsigned int size);

	TupleObject(const TupleObject &other) throw ();
	TupleObject &operator=(const TupleObject &other) throw ();

	void init_item(unsigned int index, const Object &item);

	template <typename Item, typename... Tail> void init_items(unsigned int index, Item, Tail...);
	void init_items(unsigned int index);

	unsigned int size() const;
	Object get_item(unsigned int index) const;

protected:
	struct Content;

private:
	TupleObject(BlockId id) throw ();

	Content *content() const;
};

void TupleObjectTypeInit(const TypeObject &type);

} // namespace

#include "tuple-inline.hpp"

#endif
