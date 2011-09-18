/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_FUNCTION_HPP
#define CONCRETE_OBJECTS_FUNCTION_HPP

#include <concrete/objects/callable.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/tuple.hpp>

namespace concrete {

class FunctionObject: public CallableObject {
	CONCRETE_OBJECT_DEFAULT_DECL(FunctionObject, CallableObject)

	static FunctionObject New(const CodeObject &code);

	Object continuable_call(Continuation &cont,
	                        Continuation::Stage stage,
	                        const TupleObject *args = NULL,
	                        const DictObject *kwargs = NULL) const;
};

class FunctionContinuation: public Continuation {
	friend class Pointer;

public:
	FunctionContinuation(const FunctionContinuation &other) throw (): Continuation(other) {}

	bool initiate(Object &result,
	              const TupleObject &args,
	              const DictObject &kwargs,
	              const CodeObject &code) const;

	bool resume(Object &result, const CodeObject &code) const;

private:
	struct Data;

	explicit FunctionContinuation(unsigned int address) throw (): Continuation(address) {}

	Data *data() const;
};

void FunctionObjectTypeInit(const TypeObject &type, const char *name = "function");

} // namespace

#endif
