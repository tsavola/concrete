/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "code.hpp"

#include <cstdint>

#include <concrete/exception.hpp>
#include <concrete/objects.hpp>
#include <concrete/util/loader.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

class CodeLoaderState: Noncopyable {
public:
	CodeLoaderState(const uint8_t *data, size_t size): m_data(data), m_size(size), m_position(0)
	{
	}

	const uint8_t *data() const
	{
		return m_data;
	}

	size_t size() const
	{
		return m_size;
	}

	size_t position() const
	{
		return m_position;
	}

	void advance(size_t offset)
	{
		m_position += offset;
	}

private:
	const uint8_t *const m_data;
	const size_t m_size;
	size_t m_position;
};

class CodeLoader: Loader<CodeLoaderState> {
public:
	explicit CodeLoader(CodeLoaderState &state): Loader<CodeLoaderState>(state)
	{
	}

	template <typename T>
	T load_next()
	{
		return load_next().require<T>();
	}

	Object load_next()
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
		default: throw Exception(StringObject::New(&type, 1));
		}

		return object;
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
		auto tuple = TupleObject::NewWithSize(length);

		for (unsigned int i = 0; i < length; i++)
			tuple.init_item(i, load_next());

		return tuple;
	}

	CodeObject load_code()
	{
		/* argcount = */ load<uint32_t>();
		/* kwonlyargcount = */ load<uint32_t>();
		/* nlocals = */ load<uint32_t>();
		auto stacksize = load<uint32_t>();
		/* flags = */ load<uint32_t>();
		auto code = load_next<BytesObject>();
		auto consts = load_next<TupleObject>();
		auto names = load_next<TupleObject>();
		auto varnames = load_next<TupleObject>();
		/* freevars = */ load_next();
		/* cellvars = */ load_next();
		/* filename = */ load_next();
		/* name = */ load_next();
		/* firstlineno = */ load<uint32_t>();
		/* lnotab = */ load_next();

		return CodeObject::New(stacksize, code, consts, names, varnames);
	}
};

CodeObject CodeBlock::Load(const void *data, size_t size)
{
	auto bytedata = reinterpret_cast<const uint8_t *> (data);

	// TODO: check magic and mtime

	CodeLoaderState state(bytedata + 8, size - 8);
	CodeLoader Loader(state);
	return Loader.load_next<CodeObject>();
}

void CodeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("code"));
}

} // namespace
