/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_NESTED_HPP
#define CONCRETE_UTIL_NESTED_HPP

#include <concrete/block.hpp>
#include <concrete/continuation.hpp>
#include <concrete/objects/callable.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

#define CONCRETE_INTERNAL_NESTED_CALL(Name, Function)                         \
	CONCRETE_INTERNAL_CONTINUABLE_ARGS(Name,                              \
	                                   concrete::NestedCallContinuable,   \
	                                   concrete::NestedContinuation,      \
	                                   Function)

struct NestedContinuable;

class NestedContinuation: public Block, Noncopyable {
	friend struct NestedContinuable;

public:
	~NestedContinuation() throw ()
	{
		if (call_id)
			callable.cast<CallableObject>().cleanup_call(call_id);
	}

private:
	PortableObject callable;
	PortableBlockId call_id;

} CONCRETE_PACKED;

class NestedContinuable: public Continuable {
protected:
	bool call_nested(const CallableObject &callable,
	                 Object &result,
	                 const TupleObject &args,
	                 const DictObject &kwargs) const
	{
		nested_state()->callable = callable;

		BlockId call_id;
		result = callable.init_call(call_id, args, kwargs);
		nested_state()->call_id = call_id;

		return call_id == NULL;
	}

	bool in_nested_call() const
	{
		return nested_state()->call_id;
	}

	bool resume_nested(Object &result) const
	{
		auto callable = nested_state()->callable.cast<CallableObject>();
		BlockId call_id = nested_state()->call_id;
		result = callable.resume_call(call_id);
		nested_state()->call_id = call_id;

		return call_id == NULL;
	}

	NestedContinuation *nested_state() const
	{
		return state_pointer<NestedContinuation>();
	}
};

class NestedCall: Noncopyable {
public:
	NestedCall(const Object &callable):
		callable(callable.require<CallableObject>()),
		args(TupleObject::New()),
		kwargs(DictObject::EmptySingleton())
	{
	}

	NestedCall(const Object &callable, const TupleObject &args):
		callable(callable.require<CallableObject>()),
		args(args),
		kwargs(DictObject::EmptySingleton())
	{
	}

	NestedCall(const Object &callable, const TupleObject &args, const DictObject &kwargs):
		callable(callable.require<CallableObject>()),
		args(args),
		kwargs(kwargs)
	{
	}

	NestedCall(const NestedCall &other) throw ():
		callable(other.callable),
		args(other.args),
		kwargs(other.kwargs)
	{
	}

	const CallableObject callable;
	const TupleObject args;
	const DictObject kwargs;
};

class NestedCallContinuable: public NestedContinuable {
	typedef NestedCall (*Function)(const TupleObject &args, const DictObject &kwargs);

public:
	explicit NestedCallContinuable(Function function) throw ():
		m_function(function)
	{
	}

	bool call(Object &result, const TupleObject &args, const DictObject &kwargs) const
	{
		auto call = m_function(args, kwargs);
		return call_nested(call.callable, result, call.args, call.kwargs);
	}

	bool resume(Object &result) const
	{
		return resume_nested(result);
	}

private:
	const Function m_function;
};

} // namespace

#endif
