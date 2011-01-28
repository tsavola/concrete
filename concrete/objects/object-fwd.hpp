/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_OBJECT_FWD_HPP
#define CONCRETE_OBJECTS_OBJECT_FWD_HPP

#include <concrete/block.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

struct ObjectOps {
	static const BlockId &load(const BlockId &x)  { return x; }
	static const BlockId &store(const BlockId &x) { return x; }
};

typedef portable_ops<BlockId, sizeof (BlockId)> PortableObjectOps;

template <typename Ops> class object;

typedef object<ObjectOps>         Object;
typedef object<PortableObjectOps> PortableObject;

} // namespace

#endif
