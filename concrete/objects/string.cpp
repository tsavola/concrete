/*
 * Copyright (c) 2006, 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "string-content.hpp"

#include <cstdint>
#include <cstdint>
#include <cstring>

#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void StringObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("string"));

	type.protocol().add   = InternalObject::New(internal::StringType_Add);
	type.protocol().repr  = InternalObject::New(internal::StringType_Repr);
	type.protocol().str   = InternalObject::New(internal::StringType_Str);
}

StringObject::Content::Content(const TypeObject &type, const char *data_):
	Object::Content(type)
{
	std::memcpy(data, data_, size());
	init();
}

StringObject::Content::Content(const TypeObject &type):
	Object::Content(type)
{
}

void StringObject::Content::init()
{
	auto data_size = size();
	size_t string_length = 0;

	for (size_t i = 0; i < data_size; string_length++) {
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
			if (next > data_size)
				throw RuntimeError("invalid UTF-8 string");

			for (; i < next; i++) {
				byte = data[i];

				if ((byte & 0xc0) != 0x80)
					throw RuntimeError("invalid UTF-8 string");
			}
		}
	}

	data[data_size] = '\0';

	length = string_length;
}

size_t StringObject::Content::size() const throw ()
{
	return block_size() - sizeof (Content) - 1;
}

TypeObject StringObject::Type()
{
	return Context::SystemObjects()->string_type;
}

StringObject StringObject::New(const char *data, size_t size)
{
	return Context::NewCustomSizeBlock<Content>(sizeof (Content) + size + 1, Type(), data);
}

StringObject StringObject::New(const char *string)
{
	return New(string, std::strlen(string));
}

StringObject StringObject::New(const std::string &string)
{
	return New(string.data(), string.length());
}

StringObject StringObject::NewUninitialized(size_t size)
{
	return Context::NewCustomSizeBlock<Content>(sizeof (Content) + size + 1, Type());
}

StringObject::StringObject(BlockId id) throw ():
	Object(id)
{
}

StringObject::StringObject(const StringObject &other) throw ():
	Object(other)
{
}

StringObject &StringObject::operator=(const StringObject &other) throw ()
{
	Object::operator=(other);
	return *this;
}

void StringObject::init_uninitialized()
{
	content()->init();
}

bool StringObject::equals(const StringObject &other) const
{
	if (operator==(other))
		return true;

	auto b1 = content();
	auto b2 = other.content();

	return b1->size() == b1->size() && std::memcmp(b1->data, b2->data, b1->size()) == 0;
}

size_t StringObject::size() const
{
	return content()->size();
}

size_t StringObject::length() const
{
	return content()->length;
}

char *StringObject::data() const
{
	auto c = content();
	c->data[c->size()] = '\0';
	return c->data;
}

std::string StringObject::string() const
{
	auto c = content();
	return std::string(c->data, c->size());
}

StringObject::Content *StringObject::content() const
{
	return content_pointer<Content>();
}

static Object StringAdd(const TupleObject &args, const DictObject &kwargs)
{
	auto s1 = args.get_item(0).require<StringObject>();
	auto s2 = args.get_item(1).require<StringObject>();

	auto s1size = s1.size();
	auto s2size = s2.size();

	if (s1size == 0)
		return s2;

	if (s2size == 0)
		return s1;

	auto r = StringObject::NewUninitialized(s1size + s2size);
	auto rdata = r.data();

	std::memcpy(rdata, s1.data(), s1size);
	std::memcpy(rdata + s1size, s2.data(), s2size);

	r.init_uninitialized();

	return r;
}

static Object StringRepr(const TupleObject &args, const DictObject &kwargs)
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

static Object StringStr(const TupleObject &args, const DictObject &kwargs)
{
	return args.get_item(0);
}

} // namespace

CONCRETE_INTERNAL_FUNCTION(StringType_Add,  StringAdd)
CONCRETE_INTERNAL_FUNCTION(StringType_Repr, StringRepr)
CONCRETE_INTERNAL_FUNCTION(StringType_Str,  StringStr)
