/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_TYPE_FWD_HPP
#define CONCRETE_OBJECTS_TYPE_FWD_HPP

#include <concrete/objects/object-fwd.hpp>

namespace concrete {

template <typename Ops> class TypeLogic;

typedef TypeLogic<ObjectOps>         TypeObject;
typedef TypeLogic<PortableObjectOps> PortableTypeObject;

} // namespace

#endif
