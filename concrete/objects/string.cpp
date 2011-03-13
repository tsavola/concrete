/*
 * Copyright (c) 2006, 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "string.hpp"

#include <cstdint>
#include <cstring>

#include <concrete/exception.hpp>
#include <concrete/internals.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

StringBlock::StringBlock(const TypeObject &type, const char *data_): ObjectBlock(type)
{
	auto data_length = size();

	std::memcpy(data, data_, data_length);
	data[data_length] = '\0';

	size_t string_length = 0;

	for (size_t i = 0; i < data_length; string_length++) {
		uint8_t byte = data[i++];

		if ((byte & 0x80) != 0) {
			int skip;
			size_t next;

			if ((byte & 0xe0) == 0xc0)
				skip = 1;
			else if ((byte & 0xf0) == 0xe0)
				skip = 2;
			else if ((byte & 0xf8) == 0xf0)
				skip = 3;
			else
				throw RuntimeError("invalid UTF-8 string");

			next = i + skip;
			if (next > data_length)
				throw RuntimeError("invalid UTF-8 string");

			for (; i < next; i++) {
				byte = data[i];

				if ((byte & 0xc0) != 0x80)
					throw RuntimeError("invalid UTF-8 string");
			}
		}
	}

	length = string_length;
}

CONCRETE_INTERNAL(StringObject_repr)(const TupleObject &args, const DictObject &kwargs)
{
	auto self = args.get_item(0).require<StringObject>();

	std::string value;

	value += "'";

	const char *data = self.data();
	unsigned int size = self.size();

	for (unsigned int i = 0; i < size; i++) {
		switch (data[i]) {
		case '\0':
			value += "\\0";
			break;

		case '\'':
			value += "\\'";
			break;

		case '\\':
			value += "\\\\";
			break;

		default:
			value += data[i];
			break;
		}
	}

	value += "'";

	return StringObject::New(value);
}

CONCRETE_INTERNAL(StringObject_str)(const TupleObject &args, const DictObject &kwargs)
{
	return args.get_item(0);
}

void StringInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("string"));

	type.protocol().repr  = InternalObject::New(internals::StringObject_repr);
	type.protocol().str   = InternalObject::New(internals::StringObject_str);
}

} // namespace
