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
class CallableLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	CallableLogic(const CallableLogic<OtherOps> &other): ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	CallableLogic &operator=(const CallableLogic<OtherOps> &other)
	{
		ObjectLogic<Ops>::operator=(other);
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
	CallableLogic(BlockId id): ObjectLogic<Ops>(id)
	{
	}

	CallableBlock *callable_block() const
	{
		return static_cast<CallableBlock *> (ObjectLogic<Ops>::object_block());
	}
} CONCRETE_PACKED;

typedef CallableLogic<ObjectOps>         CallableObject;
typedef CallableLogic<PortableObjectOps> PortableCallableObject;

template <>
struct TypeCheck<CallableObject> {
	bool operator()(const TypeObject &type)
	{
		return CallableBlock::Check(type);
	}
};

template <>
struct TypeCheck<PortableCallableObject> {
	bool operator()(const TypeObject &type)
	{
		return CallableBlock::Check(type);
	}
};

} // namespace

#endif
