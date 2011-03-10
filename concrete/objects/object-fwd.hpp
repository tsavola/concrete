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

template <typename Ops> class ObjectLogic;

struct ObjectOps {
	static const BlockId &Load(const BlockId &x)  { return x; }
	static const BlockId &Store(const BlockId &x) { return x; }
};

struct PortableObjectOps {
	static BlockId Load(BlockId x)  { return PortableOps<BlockId, sizeof (x)>::Load(x); }
	static BlockId Store(BlockId x) { return PortableOps<BlockId, sizeof (x)>::Store(x); }
};

typedef ObjectLogic<ObjectOps>         Object;
typedef ObjectLogic<PortableObjectOps> PortableObject;

} // namespace

#endif
