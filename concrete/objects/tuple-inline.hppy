/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

namespace concrete {

template <typename Item, typename... Tail>
TupleObject TupleObject::New(Item first_item, Tail... other_items)
{
	auto tuple = NewWithSize(1 + sizeof...(Tail));
	tuple.init_items(0, first_item, other_items...);
	return tuple;
}

template <typename Item, typename... Tail>
void TupleObject::init_items(unsigned int first_index, Item first_item, Tail... other_items)
{
	init_item(first_index, first_item);
	init_items(first_index + 1, other_items...);
}

inline void TupleObject::init_items(unsigned int first_index)
{
}

} // namespace
