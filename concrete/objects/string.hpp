/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_STRING_HPP
#define CONCRETE_OBJECTS_STRING_HPP

#include <cstddef>
#include <string>

#include <concrete/objects/object-partial.hpp>

namespace concrete {

class StringObject: public Object {
	friend class Pointer;
	friend class Object;

public:
	static TypeObject Type();

	static StringObject New(const char *data, size_t size);
	static StringObject New(const char *string);
	static StringObject New(const std::string &string);
	static StringObject NewUninitialized(size_t size);

	StringObject(const StringObject &other) throw (): Object(other) {}

	void init_uninitialized();

	bool equals(const StringObject &other) const;
	size_t size() const;
	size_t length() const;
	char *c_str() const;
	std::string string() const;

protected:
	struct Data;

	explicit StringObject(unsigned int address) throw (): Object(address) {}

private:
	Data *data() const;
};

void StringObjectTypeInit(const TypeObject &type);

} // namespace

#endif
