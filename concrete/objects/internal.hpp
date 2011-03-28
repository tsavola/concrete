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
#include <concrete/objects/callable.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>

namespace concrete {

typedef Object InternalFunction(ContinuationOp op,
                                BlockId &continuation,
                                const TupleObject *args,
                                const DictObject *kwargs);

namespace internal {
	enum SymbolId {
#		define CONCRETE_INTERNAL_SYMBOL(Symbol)  Symbol,
#		include <concrete/internals.hpp>
#		undef CONCRETE_INTERNAL_SYMBOL
	};
}

namespace internal_symbol {
#	define CONCRETE_INTERNAL_SYMBOL(Symbol)  InternalFunction Symbol;
#	include <concrete/internals.hpp>
#	undef CONCRETE_INTERNAL_SYMBOL
}

#define CONCRETE_INTERNAL_FUNCTION(Symbol, Function)                          \
	concrete::Object concrete::internal_symbol::Symbol(                   \
		ContinuationOp op,                                            \
		BlockId &,                                                    \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		return Function(*args, *kwargs);                              \
	}

#define CONCRETE_INTERNAL_CONTINUABLE(Symbol, Continuable, Continuation, ...) \
	concrete::Object concrete::internal_symbol::Symbol(                   \
		ContinuationOp op,                                            \
		BlockId &continuation,                                        \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		return ContinuableCall<Continuation>(                         \
			op,                                                   \
			continuation,                                         \
			Continuable(__VA_ARGS__),                             \
			args,                                                 \
			kwargs);                                              \
	}

struct InternalBlock: CallableBlock {
	const Portable<uint16_t> symbol_id;

	InternalBlock(const TypeObject &type, internal::SymbolId symbol_id):
		CallableBlock(type),
		symbol_id(symbol_id)
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
		return Context::SystemObjects()->internal_type;
	}

	static InternalLogic New(internal::SymbolId symbol_id)
	{
		return Context::NewBlock<InternalBlock>(Type(), symbol_id);
	}

	using CallableLogic<Ops>::operator==;
	using CallableLogic<Ops>::operator!=;

	template <typename OtherOps>
	InternalLogic(const InternalLogic<OtherOps> &other) throw ():
		CallableLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	InternalLogic &operator=(const InternalLogic<OtherOps> &other) throw ()
	{
		CallableLogic<Ops>::operator=(other);
		return *this;
	}

	template <typename... Args>
	Object call(Args... args)
	{
		BlockId continuation;

		auto tuple = TupleObject::New(args...);
		auto dict = DictObject::EmptySingleton();
		auto value = internal_block()->call(InitContinuation, continuation, &tuple, &dict);

		if (continuation) {
			internal_block()->call(CleanupContinuation, continuation, NULL, NULL);
			throw RuntimeError("non-trivial protocol method called from native code");
		}

		return value;
	}

protected:
	InternalLogic(BlockId id) throw ():
		CallableLogic<Ops>(id)
	{
	}

	InternalBlock *internal_block() const
	{
		return static_cast<InternalBlock *> (CallableLogic<Ops>::callable_block());
	}
} CONCRETE_PACKED;

typedef InternalLogic<ObjectOps>         InternalObject;
typedef InternalLogic<PortableObjectOps> PortableInternalObject;

void InternalTypeInit(const TypeObject &type);

} // namespace

#endif
