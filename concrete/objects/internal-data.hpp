/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_INTERNAL_DATA_HPP
#define CONCRETE_OBJECTS_INTERNAL_DATA_HPP

#include "internal.hpp"

#include <cstdint>

#include <concrete/continuation.hpp>
#include <concrete/objects/object-data.hpp>
#include <concrete/portable.hpp>

namespace concrete {

struct InternalObject::Data: CallableObject::Data {
	Data(const TypeObject &type, internal::SymbolId symbol_id);

	const Portable<uint16_t> symbol_id;
} CONCRETE_PACKED;

} // namespace

#endif
