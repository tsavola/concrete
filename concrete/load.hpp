/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_LOAD_HPP
#define CONCRETE_LOAD_HPP

#include <cstddef>

#include <concrete/objects/code.hpp>

namespace concrete {

CodeObject load_code(const void *data, size_t size);
CodeObject load_code_file(const char *filename);

} // namespace

#endif
