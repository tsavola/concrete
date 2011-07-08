/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_LONG_HPP
#define CONCRETE_OBJECTS_LONG_HPP

#include <cstdint>

#include <concrete/objects/object.hpp>

namespace concrete {

class LongObject: public Object {
	friend class Pointer;
	friend class Object;

public:
	static TypeObject Type();
	static LongObject New(int64_t value);

	LongObject(const LongObject &other) throw (): Object(other) {}

	int64_t value() const;

protected:
	struct Data;

	explicit LongObject(unsigned int address) throw (): Object(address) {}

private:
	Data *data() const;
};

void LongObjectTypeInit(const TypeObject &type, const char *name = "long");

} // namespace

#endif
