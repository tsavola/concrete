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
#include <concrete/objects/tuple.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

typedef internals::Serial InternalSerial;

typedef Object (*InternalFunction)(ContinuationOp op,
                                   BlockId &continuation,
                                   const TupleObject *args,
                                   const DictObject *kwargs);

#define CONCRETE_INTERNAL(function)                                           \
	static Object function(const TupleObject &, const DictObject &);      \
	                                                                      \
	static Object function##__entry(                                      \
		ContinuationOp op,                                            \
		BlockId &,                                                    \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		assert(op == InitContinuation);                               \
		return function(*args, *kwargs);                              \
	}                                                                     \
	                                                                      \
	static void function##__register() __attribute__ ((constructor));     \
	                                                                      \
	void function##__register()                                           \
	{                                                                     \
		register_internal(internals::function, function##__entry);    \
	}                                                                     \
	                                                                      \
	Object function

#define CONCRETE_INTERNAL_CONTINUABLE(Continuable, Continuation)              \
	struct Continuable;                                                   \
	                                                                      \
	static Object Continuable##__entry(                                   \
		ContinuationOp op,                                            \
		BlockId &id,                                                  \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		return continuable_call<Continuation>(                        \
			op, id, Continuable(), args, kwargs);                 \
	}                                                                     \
	                                                                      \
	static void Continuable##__register() __attribute__ ((constructor));  \
	                                                                      \
	void Continuable##__register()                                        \
	{                                                                     \
		register_internal(                                            \
			internals::Continuable,                               \
			Continuable##__entry);                                \
	}                                                                     \
	                                                                      \
	struct Continuable

struct InternalBlock: CallableBlock {
	const portable<uint16_t> serial;

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
class internal_object: public callable_object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().internal_type;
	}

	static internal_object New(InternalSerial serial) throw (AllocError)
	{
		auto id = Context::Alloc(sizeof (InternalBlock));
		new (Context::Pointer(id)) InternalBlock(Type(), serial);
		return id;
	}

	using callable_object<Ops>::operator==;
	using callable_object<Ops>::operator!=;

	template <typename OtherOps>
	internal_object(const internal_object<OtherOps> &other): callable_object<Ops>(other)
	{
	}

	template <typename OtherOps>
	internal_object &operator=(const internal_object<OtherOps> &other)
	{
		callable_object<Ops>::operator=(other);
		return *this;
	}

protected:
	internal_object(BlockId id): callable_object<Ops>(id)
	{
	}

	InternalBlock *internal_block() const
	{
		return static_cast<InternalBlock *> (callable_object<Ops>::callable_block());
	}
} CONCRETE_PACKED;

typedef internal_object<ObjectOps>         InternalObject;
typedef internal_object<PortableObjectOps> PortableInternalObject;

void register_internal(InternalSerial serial, InternalFunction function);

} // namespace

#endif
