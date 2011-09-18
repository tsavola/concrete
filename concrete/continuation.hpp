/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_CONTINUATION_HPP
#define CONCRETE_CONTINUATION_HPP

#include <concrete/pointer.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>

namespace concrete {

class Continuation: public Pointer {
	CONCRETE_POINTER_DECL_COMMON(Continuation, Pointer)
	CONCRETE_POINTER_DECL_CONSTRUCT(Continuation)

	enum Stage {
		Initiate,
		Resume,
		Release,
	};

	template <typename ImplType, typename... ImplParams>
	static Object Call(Continuation &cont,
	                   Stage stage,
	                   const TupleObject *args,
	                   const DictObject *kwargs,
	                   ImplParams... impl_params);
};

#define CONCRETE_CONTINUATION_DEFAULT_DECL(Class, ParentClass)                \
	CONCRETE_POINTER_DECL_COMMON(Class, ParentClass)                      \
	CONCRETE_POINTER_DECL_DATA(Class)

#define CONCRETE_CONTINUATION_DEFAULT_IMPL(Class)                             \
	CONCRETE_POINTER_IMPL_DATA(Class)

} // namespace

#include "continuation-inline.hpp"

#endif
