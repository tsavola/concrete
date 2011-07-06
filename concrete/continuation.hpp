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
	friend class Pointer;

public:
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

	Continuation() throw () {}
	Continuation(const Continuation &other) throw (): Pointer(other) {}

protected:
	explicit Continuation(unsigned int address) throw (): Pointer(address) {}
};

} // namespace

#include "continuation-inline.hpp"

#endif
