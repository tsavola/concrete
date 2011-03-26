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

#include <concrete/block.hpp>
#include <concrete/continuation.hpp>
#include <concrete/objects/callable.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct FunctionBlock: CallableBlock {
	const PortableCodeObject code;

	FunctionBlock(const TypeObject &type, const CodeObject &code):
		CallableBlock(type),
		code(code)
	{
	}

	Object call(ContinuationOp op,
	            BlockId &continuation,
	            const TupleObject *args,
	            const DictObject *kwargs) const;
} CONCRETE_PACKED;

template <typename Ops>
class FunctionLogic: public CallableLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::BuiltinObjects().function_type;
	}

	static FunctionLogic New(const CodeObject &code)
	{
		return Context::NewBlock<FunctionBlock>(Type(), code);
	}

	using CallableLogic<Ops>::operator==;
	using CallableLogic<Ops>::operator!=;

	template <typename OtherOps>
	FunctionLogic(const FunctionLogic<OtherOps> &other) throw ():
		CallableLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	FunctionLogic &operator=(const FunctionLogic<OtherOps> &other) throw ()
	{
		CallableLogic<Ops>::operator=(other);
		return *this;
	}

protected:
	FunctionLogic(BlockId id) throw ():
		CallableLogic<Ops>(id)
	{
	}

	FunctionBlock *function_block() const
	{
		return static_cast<FunctionBlock *> (CallableLogic<Ops>::callable_block());
	}
} CONCRETE_PACKED;

typedef FunctionLogic<ObjectOps>         FunctionObject;
typedef FunctionLogic<PortableObjectOps> PortableFunctionObject;

void FunctionTypeInit(const TypeObject &type);

} // namespace

#endif
