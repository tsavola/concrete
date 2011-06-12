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

namespace concrete {

class FunctionObject: public CallableObject {
	friend class Object;
	friend class CallableObject;

public:
	static TypeObject Type();
	static FunctionObject New(const CodeObject &code);

	FunctionObject(const FunctionObject &other) throw ();
	FunctionObject &operator=(const FunctionObject &other) throw ();

protected:
	struct Content;

	FunctionObject(BlockId id) throw ();

private:
	Content *content() const;
};

void FunctionObjectTypeInit(const TypeObject &type);

} // namespace

#endif
