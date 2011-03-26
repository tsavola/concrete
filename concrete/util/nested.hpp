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
	CONCRETE_INTERNAL_CONTINUABLE(Name,                                   \
	                              ::concrete::NestedCallContinuable,      \
	                              ::concrete::NestedContinuation,         \
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

struct NestedContinuable {
protected:
	bool call_nested(BlockId state_id,
	                 const CallableObject &callable,
	                 Object &result,
	                 const TupleObject &args,
	                 const DictObject &kwargs) const
	{
		NestedState(state_id)->callable = callable;

		BlockId call_id;
		result = callable.init_call(call_id, args, kwargs);
		NestedState(state_id)->call_id = call_id;

		return call_id == NULL;
	}

	bool in_nested_call(BlockId state_id) const
	{
		return NestedState(state_id)->call_id;
	}

	bool resume_nested(BlockId state_id, Object &result) const
	{
		auto callable = NestedState(state_id)->callable.cast<CallableObject>();
		BlockId call_id = NestedState(state_id)->call_id;
		result = callable.resume_call(call_id);
		NestedState(state_id)->call_id = call_id;

		return call_id == NULL;
	}

	static NestedContinuation *NestedState(BlockId id)
	{
		return Context::BlockPointer<NestedContinuation>(id);
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

struct NestedCallContinuable: NestedContinuable {
	typedef NestedCall (*Function)(const TupleObject &args, const DictObject &kwargs);

	const Function function;

	explicit NestedCallContinuable(Function function): function(function)
	{
	}

	bool call(BlockId state_id,
	          Object &result,
	          const TupleObject &args,
	          const DictObject &kwargs) const
	{
		auto call = function(args, kwargs);
		return call_nested(state_id, call.callable, result, call.args, call.kwargs);
	}

	bool resume(BlockId state_id, Object &result) const
	{
		return resume_nested(state_id, result);
	}
};

} // namespace

#endif
