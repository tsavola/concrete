/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_FUNCTION_DATA_HPP
#define CONCRETE_OBJECTS_FUNCTION_DATA_HPP

#include "concrete/objects/function.hpp"

#include <concrete/continuation.hpp>
#include <concrete/execution.hpp>
#include <concrete/objects/object-data.hpp>
#include <concrete/portable.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

struct FunctionObject::Data: CallableObject::Data {
	Data(const TypeObject &type, const CodeObject &code) throw ();

	const Portable<CodeObject> code;
} CONCRETE_PACKED;

struct FunctionContinuation::Data: Noncopyable {
	~Data() throw ();

	Portable<ExecutionFrame> frame;
} CONCRETE_PACKED;

} // namespace

#endif
