/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_NONE_DECL_HPP
#define CONCRETE_OBJECTS_NONE_DECL_HPP

#include "none-fwd.hpp"

#include <concrete/objects/object-decl.hpp>
#include <concrete/objects/type-fwd.hpp>

namespace concrete {

struct NoneBlock;

class NoneObject: public object<ObjectOps> {
public:
	static TypeObject Type();
	static NoneObject NewBuiltin() throw (AllocError);

	NoneObject()
	{
	}

	NoneObject(const NoneObject &other): object<ObjectOps>(other)
	{
	}

	NoneObject &operator=(NoneObject &other)
	{
		object<ObjectOps>::operator=(other);
		return *this;
	}

	void init_builtin(const TypeObject &type);

private:
	NoneObject(BlockId id): object<ObjectOps>(id)
	{
	}
};

} // namespace

#endif
