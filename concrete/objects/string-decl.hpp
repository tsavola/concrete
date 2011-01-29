/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_STRING_DECL_HPP
#define CONCRETE_OBJECTS_STRING_DECL_HPP

#include "string-fwd.hpp"

#include <cstddef>
#include <cstring>

#include <concrete/objects/object-decl.hpp>
#include <concrete/objects/type-fwd.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct StringBlock;

template <typename Ops>
class string_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type();

	static string_object New(const char *string) throw (AllocError)
	{
		return New(string, std::strlen(string));
	}

	static string_object New(const char *data, size_t size) throw (AllocError);

	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	string_object(const string_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	string_object &operator=(const string_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
		return *this;
	}

	template <typename OtherOps>
	bool equals(const string_object<OtherOps> &other) const
	{
		return operator==(other) || std::strcmp(data(), other.data()) == 0;
	}

	size_t size() const;
	size_t length() const;
	const char *data() const;

protected:
	string_object(BlockId id): object<Ops>(id)
	{
	}

	StringBlock *string_block() const;

} CONCRETE_PACKED;

} // namespace

#endif
