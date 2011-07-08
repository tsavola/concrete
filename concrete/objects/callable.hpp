/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_CALLABLE_HPP
#define CONCRETE_OBJECTS_CALLABLE_HPP

#include <cstddef>

#include <concrete/continuation.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>

namespace concrete {

class CallableObject: public Object {
	friend class Pointer;

public:
	CallableObject(const CallableObject &other) throw (): Object(other) {}

	Object initiate(Continuation &cont, const TupleObject &args, const DictObject &kwargs) const;
	Object resume(Continuation &cont) const;
	void release(Continuation cont) const throw ();

	Object continuable_call(Continuation &cont,
	                        Continuation::Stage stage,
	                        const TupleObject *args = NULL,
	                        const DictObject *kwargs = NULL) const;

protected:
	explicit CallableObject(unsigned int address) throw (): Object(address) {}
};

template <> struct TypeCheck<CallableObject> {
	bool operator()(const TypeObject &type);
};

void CallableObjectTypeInit(const TypeObject &type, const char *name);

} // namespace

#endif
