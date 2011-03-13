/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_MODULES_BUILTINS_HPP
#define CONCRETE_MODULES_BUILTINS_HPP

#include <concrete/objects/dict.hpp>

namespace concrete {

DictObject BuiltinsModuleInit(const DictObject &modules);

} // namespace

#endif
