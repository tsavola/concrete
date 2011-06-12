/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_DICT_CONTENT_HPP
#define CONCRETE_OBJECTS_DICT_CONTENT_HPP

#include "dict.hpp"

#include <concrete/objects/object-content.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

struct DictObject::Content: Object::Content {
	struct Item: Noncopyable {
		Portable<Object> key;
		Portable<Object> value;

		Item(const Object &key, const Object &value);

	} CONCRETE_PACKED;

	Portable<uint32_t> size;
	Item items[0];

	explicit Content(const TypeObject &type);
	~Content() throw ();

	void verify_integrity() const;
	unsigned int capacity() const throw ();
	unsigned int find_item(const Object &key) const;

} CONCRETE_PACKED;

} // namespace

#endif
