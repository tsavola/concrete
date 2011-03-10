/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_BACKTRACE_HPP
#define CONCRETE_UTIL_BACKTRACE_HPP

namespace concrete {

#ifdef CONCRETE_BACKTRACE
void Backtrace();
#else
inline void Backtrace() {}
#endif

} // namespace

#endif
