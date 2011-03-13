/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_INTERNAL_HPP
#define CONCRETE_OBJECTS_INTERNAL_HPP

#include <cassert>
#include <cstdint>

#include <concrete/block.hpp>
#include <concrete/context.hpp>
#include <concrete/continuation.hpp>
#include <concrete/internals.hpp>
#include <concrete/objects/callable.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

typedef internals::Serial InternalSerial;

typedef Object (*InternalFunction)(ContinuationOp op,
                                   BlockId &continuation,
                                   const TupleObject *args,
                                   const DictObject *kwargs);

#define CONCRETE_INTERNAL_FUNCTION_DECLARE(Function)                          \
	static ::concrete::Object Function(const ::concrete::TupleObject &,   \
	                                   const ::concrete::DictObject &);   \
	                                                                      \
	static ::concrete::Object Function##__entry(                          \
		::concrete::ContinuationOp op,                                \
		::concrete::BlockId &,                                        \
		const ::concrete::TupleObject *args,                          \
		const ::concrete::DictObject *kwargs)                         \
	{                                                                     \
		assert(op == ::concrete::InitContinuation);                   \
		return Function(*args, *kwargs);                              \
	}

#define CONCRETE_INTERNAL_FUNCTION_IMPLEMENT(Function)                        \
	::concrete::Object Function

#define CONCRETE_INTERNAL_FUNCTION(Function)                                  \
	CONCRETE_INTERNAL_FUNCTION_DECLARE(Function)                          \
	CONCRETE_INTERNAL_FUNCTION_IMPLEMENT(Function)

#define CONCRETE_INTERNAL_REGISTER(Internal, Function)                        \
	static void Internal##__register() __attribute__ ((constructor));     \
	                                                                      \
	void Internal##__register()                                           \
	{                                                                     \
		::concrete::InternalRegister(::concrete::internals::Internal, \
		                             Function##__entry);              \
	}

#define CONCRETE_INTERNAL(Function)                                           \
	CONCRETE_INTERNAL_FUNCTION_DECLARE(Function)                          \
	CONCRETE_INTERNAL_REGISTER(Function, Function)                        \
	CONCRETE_INTERNAL_FUNCTION_IMPLEMENT(Function)

#define CONCRETE_INTERNAL_CONTINUABLE(Continuable, Continuation)              \
	struct Continuable;                                                   \
	                                                                      \
	static ::concrete::Object Continuable##__entry(                       \
		::concrete::ContinuationOp op,                                \
		::concrete::BlockId &continuation,                            \
		const ::concrete::TupleObject *args,                          \
		const ::concrete::DictObject *kwargs)                         \
	{                                                                     \
		return ::concrete::ContinuableCall<Continuation>(             \
			op, continuation, Continuable(), args, kwargs);       \
	}                                                                     \
	                                                                      \
	static void Continuable##__register() __attribute__ ((constructor));  \
	                                                                      \
	void Continuable##__register()                                        \
	{                                                                     \
		::concrete::InternalRegister(                                 \
			::concrete::internals::Continuable,                   \
			Continuable##__entry);                                \
	}                                                                     \
	                                                                      \
	struct Continuable

struct InternalBlock: CallableBlock {
	const Portable<uint16_t> serial;

	InternalBlock(const TypeObject &type, InternalSerial serial):
		CallableBlock(type),
		serial(serial)
	{
	}

	Object call(ContinuationOp op,
	            BlockId &continuation,
	            const TupleObject *args,
	            const DictObject *kwargs) const;
} CONCRETE_PACKED;

template <typename Ops>
class InternalLogic: public CallableLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().internal_type;
	}

	static InternalLogic New(InternalSerial serial)
	{
		return Context::NewBlock<InternalBlock>(Type(), serial);
	}

	using CallableLogic<Ops>::operator==;
	using CallableLogic<Ops>::operator!=;

	template <typename OtherOps>
	InternalLogic(const InternalLogic<OtherOps> &other): CallableLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	InternalLogic &operator=(const InternalLogic<OtherOps> &other)
	{
		CallableLogic<Ops>::operator=(other);
		return *this;
	}

	template <typename... Args>
	Object call(Args... args)
	{
		BlockId continuation = NoBlockId;
		auto tuple = TupleObject::NewFromItems(args...);
		auto dict = DictObject::New(0);
		auto value = internal_block()->call(InitContinuation, continuation, &tuple, &dict);

		if (continuation != NoBlockId) {
			internal_block()->call(CleanupContinuation, continuation, NULL, NULL);
			throw RuntimeError("non-trivial protocol method called from native code");
		}

		return value;
	}

protected:
	InternalLogic(BlockId id): CallableLogic<Ops>(id)
	{
	}

	InternalBlock *internal_block() const
	{
		return static_cast<InternalBlock *> (CallableLogic<Ops>::callable_block());
	}
} CONCRETE_PACKED;

typedef InternalLogic<ObjectOps>         InternalObject;
typedef InternalLogic<PortableObjectOps> PortableInternalObject;

void InternalRegister(InternalSerial serial, InternalFunction function);
void InternalInit(const TypeObject &type);

} // namespace

#endif
