/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_CODE_HPP
#define CONCRETE_OBJECTS_CODE_HPP

#include <cstddef>

#include <concrete/objects/bytes.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

class CodeObject: public Object {
	friend class Object;

public:
	static TypeObject Type();
	static CodeObject New(unsigned int stacksize,
	                      const BytesObject &code,
	                      const TupleObject &consts,
	                      const TupleObject &names,
	                      const TupleObject &varnames);
	static CodeObject Load(const void *data, size_t size);

	CodeObject(const CodeObject &other) throw ();
	CodeObject &operator=(const CodeObject &other) throw ();

	unsigned int stacksize() const;
	BytesObject code() const;
	TupleObject consts() const;
	TupleObject names() const;
	TupleObject varnames() const;

protected:
	struct Content;

private:
	CodeObject(BlockId id) throw ();

	Content *content() const;
};

void CodeObjectTypeInit(const TypeObject &type);

} // namespace

#endif
