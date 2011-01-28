/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "load.hpp"

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>

#include <boost/scoped_array.hpp>

#include <concrete/block.hpp>
#include <concrete/execute.hpp>
#include <concrete/objects/bytes.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/loader.hpp>

namespace concrete {

class ObjectLoader: Loader
{
public:
	ObjectLoader(const uint8_t *data, size_t size): Loader(data + 8, size - 8)
	{
		// TODO: magic = reinterpret_cast<const uint32_t *> (data)[0];
		// TODO: mtime = reinterpret_cast<const uint32_t *> (data)[1];
	}

	Object load_object()
	{
		Object object;

		auto type = load<char>();

		switch (type) {
		case 'N': break;
		case 'i': object = load_int(); break;
		case 's': object = load_string(); break;
		case '(': object = load_tuple(); break;
		case 'c': object = load_code(); break;
		case 'u': object = load_unicode(); break;
		default: throw std::runtime_error("unsupported object type");
		}

		return object;
	}

	template <typename T>
	T load_object(const char *error)
	{
		Object object = load_object();

		if (!object.check<T>())
			throw std::runtime_error(error);

		return object.cast<T>();
	}

private:
	LongObject load_int()
	{
		return LongObject::New(load<int32_t>());
	}

	BytesObject load_string()
	{
		auto length = load<uint32_t>();
		return BytesObject::New(load_raw<uint8_t>(length), length);
	}

	StringObject load_unicode()
	{
		auto size = load<uint32_t>();
		return StringObject::New(load_chars(size), size);
	}

	TupleObject load_tuple()
	{
		auto length = load<uint32_t>();
		auto tuple = TupleObject::New(length);

		for (unsigned int i = 0; i < length; i++)
			tuple.init_item(i, load_object());

		return tuple;
	}

	CodeObject load_code()
	{
		/* argcount = */ load<uint32_t>();
		/* kwonlyargcount = */ load<uint32_t>();
		/* nlocals = */ load<uint32_t>();
		auto stacksize = load<uint32_t>();
		/* flags = */ load<uint32_t>();
		auto code = load_object<BytesObject>("code must be a bytes object");
		auto consts = load_object<TupleObject>("consts must be a tuple object");
		auto names = load_object<TupleObject>("names must be a tuple object");
		auto varnames = load_object<TupleObject>("varnames must be a tuple object");
		/* freevars = */ load_object();
		/* cellvars = */ load_object();
		/* filename = */ load_object();
		/* name = */ load_object();
		/* firstlineno = */ load<uint32_t>();
		/* lnotab = */ load_object();

		return CodeObject::New(stacksize, code, consts, names, varnames);
	}
};

CodeObject load_code(const void *data, size_t size)
{
	ObjectLoader loader(reinterpret_cast<const uint8_t *> (data), size);
	return loader.load_object<CodeObject>("code object expected");
}

CodeObject load_code_file(const char *filename)
{
	std::ifstream stream;

	stream.exceptions(std::ios::failbit | std::ios::badbit);
	stream.open(filename);

	stream.seekg(0, std::ios::end);
	auto size = stream.tellg();
	stream.seekg(0, std::ios::beg);

	boost::scoped_array<char> buf(new char[size]);

	stream.read(buf.get(), size);
	stream.close();

	return load_code(buf.get(), size);
}

} // namespace
