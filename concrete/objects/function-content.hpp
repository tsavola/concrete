/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_FUNCTION_CONTENT_HPP
#define CONCRETE_OBJECTS_FUNCTION_CONTENT_HPP

#include "function.hpp"

#include <concrete/continuation.hpp>
#include <concrete/objects/callable-content.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

struct FunctionObject::Content: CallableObject::Content {
	const Portable<CodeObject> code;

	Content(const TypeObject &type, const CodeObject &code);

	Object call(ContinuationOp op,
	            BlockId &continuation,
	            const TupleObject *args,
	            const DictObject *kwargs) const;
} CONCRETE_PACKED;

} // namespace

#endif
