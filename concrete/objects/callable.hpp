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
	friend class Object;

public:
	CallableObject(const CallableObject &other) throw ();
	CallableObject &operator=(const CallableObject &other) throw ();

	Object init_call(BlockId &continuation,
	                 const TupleObject &args,
	                 const DictObject &kwargs) const;
	Object resume_call(BlockId &continuation) const;
	void cleanup_call(BlockId continuation) const throw ();

protected:
	struct Content;

	CallableObject(BlockId id) throw ();

private:
	Object call(ContinuationOp op,
	            BlockId &continuation,
	            const TupleObject *args = NULL,
	            const DictObject *kwargs = NULL) const;

	Content *content() const;
};

template <>
struct TypeCheck<CallableObject> {
	bool operator()(const TypeObject &type);
};

} // namespace

#endif
