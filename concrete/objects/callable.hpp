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

#include <concrete/block.hpp>
#include <concrete/continuation.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct CallableBlock: ObjectBlock {
	CallableBlock(const TypeObject &type): ObjectBlock(type)
	{
	}

	Object call(ContinuationOp op,
	            BlockId &continuation,
	            const TupleObject *args = NULL,
	            const DictObject *kwargs = NULL) const;

	static bool Check(const TypeObject &type);

} CONCRETE_PACKED;

template <typename Ops>
class callable_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	callable_object(const callable_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	callable_object &operator=(const callable_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
		return *this;
	}

	Object init_call(BlockId &continuation,
	                 const TupleObject &args,
	                 const DictObject &kwargs) const
	{
		return callable_block()->call(InitContinuation, continuation, &args, &kwargs);
	}

	Object resume_call(BlockId &continuation) const
	{
		return callable_block()->call(ResumeContinuation, continuation);
	}

	void cleanup_call(BlockId cont) const
	{
		callable_block()->call(CleanupContinuation, cont);
	}

protected:
	callable_object(BlockId id): object<Ops>(id)
	{
	}

	CallableBlock *callable_block() const
	{
		return static_cast<CallableBlock *> (object<Ops>::object_block());
	}
} CONCRETE_PACKED;

typedef callable_object<ObjectOps>         CallableObject;
typedef callable_object<PortableObjectOps> PortableCallableObject;

template <>
struct type_check<CallableObject> {
	bool operator()(const TypeObject &type)
	{
		return CallableBlock::Check(type);
	}
};

template <>
struct type_check<PortableCallableObject> {
	bool operator()(const TypeObject &type)
	{
		return CallableBlock::Check(type);
	}
};

} // namespace

#endif
