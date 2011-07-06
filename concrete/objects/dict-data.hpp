/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_DICT_DATA_HPP
#define CONCRETE_OBJECTS_DICT_DATA_HPP

#include "dict.hpp"

#include <concrete/objects/object-data.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct DictObject::Data: Object::Data {
	struct Item: Noncopyable {
		Item(const Object &key, const Object &value);

		Portable<Object> key;
		Portable<Object> value;
	} CONCRETE_PACKED;

	explicit Data(const TypeObject &type);
	~Data() throw ();

	unsigned int capacity() const throw ();
	unsigned int find_item(const Object &key) const;

	Portable<uint32_t> size;
	Item               items[0];
} CONCRETE_PACKED;

} // namespace

#endif