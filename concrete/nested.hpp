/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_NESTED_HPP
#define CONCRETE_NESTED_HPP

#include <concrete/continuation.hpp>
#include <concrete/objects/callable.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

class NestedContinuation: public Continuation {
	friend class Pointer;

public:
	NestedContinuation() throw () {}
	NestedContinuation(const NestedContinuation &other) throw (): Continuation(other) {}

protected:
	struct Data;

	explicit NestedContinuation(unsigned int address) throw (): Continuation(address) {}

	bool call_nested(const CallableObject &callable,
	                 Object &result,
	                 const TupleObject &args,
	                 const DictObject &kwargs) const;

	bool in_nested_call() const;

	bool resume_nested(Object &result) const;

private:
	Data *data() const;
};

class NestedCall: Noncopyable {
public:
	NestedCall(const Object &callable);
	NestedCall(const Object &callable, const TupleObject &args);
	NestedCall(const Object &callable, const TupleObject &args, const DictObject &kwargs);

	NestedCall(const NestedCall &other) throw ();

	const CallableObject callable;
	const TupleObject    args;
	const DictObject     kwargs;
};

class NestedCallContinuation: public NestedContinuation {
	friend class Pointer;

public:
	typedef NestedCall (*DeclaratorFunction)(const TupleObject &args, const DictObject &kwargs);

	bool initiate(Object &result,
	              const TupleObject &args,
	              const DictObject &kwargs,
	              DeclaratorFunction declare) const;

	bool resume(Object &result, DeclaratorFunction declare) const;

protected:
	explicit NestedCallContinuation(unsigned int address) throw (): NestedContinuation(address) {}
};

} // namespace

#endif
