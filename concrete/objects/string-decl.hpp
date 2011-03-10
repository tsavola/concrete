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
class StringLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type();

	static StringLogic New(const char *string)
	{
		return New(string, std::strlen(string));
	}

	static StringLogic New(const char *data, size_t size);

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	StringLogic(const StringLogic<OtherOps> &other): ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	StringLogic &operator=(const StringLogic<OtherOps> &other)
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	template <typename OtherOps>
	bool equals(const StringLogic<OtherOps> &other) const
	{
		return operator==(other) || std::strcmp(data(), other.data()) == 0;
	}

	size_t size() const;
	size_t length() const;
	const char *data() const;

protected:
	StringLogic(BlockId id): ObjectLogic<Ops>(id)
	{
	}

	StringBlock *string_block() const;

} CONCRETE_PACKED;

} // namespace

#endif
