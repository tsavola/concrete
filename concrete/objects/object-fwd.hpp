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

namespace concrete {

typedef BlockIdOps         ObjectOps;
typedef PortableBlockIdOps PortableObjectOps;

template <typename Ops> class ObjectLogic;

typedef ObjectLogic<ObjectOps>         Object;
typedef ObjectLogic<PortableObjectOps> PortableObject;

} // namespace

#endif
