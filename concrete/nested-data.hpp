/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_NESTED_DATA_HPP
#define CONCRETE_NESTED_DATA_HPP

#include "concrete/nested.hpp"

#include <concrete/portable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct NestedContinuation::Data: Noncopyable {
	~Data() throw ();

	Portable<Object>       callable;
	Portable<Continuation> nested;
} CONCRETE_PACKED;

} // namespace

#define CONCRETE_INTERNAL_NESTED_CALL(Name, DeclaratorFunction)               \
	CONCRETE_INTERNAL_CONTINUATION_PARAMS(                                \
		Name, concrete::NestedCallContinuation, DeclaratorFunction)   \

#endif
