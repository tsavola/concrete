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

#include <concrete/continuation.hpp>
#include <concrete/objects/callable.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/tuple.hpp>

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

class InternalObject: public CallableObject {
	friend class Object;
	friend class CallableObject;

public:
	static TypeObject Type();
	static InternalObject New(internal::SymbolId symbol_id);

	InternalObject(const InternalObject &other) throw ();
	InternalObject &operator=(const InternalObject &other) throw ();

	template <typename... Args> Object call(Args... args) const;

protected:
	struct Content;

	InternalObject(BlockId id) throw ();

private:
	Object call_args(const TupleObject &args) const;
	Content *content() const;
};

void InternalObjectTypeInit(const TypeObject &type);

} // namespace

#define CONCRETE_INTERNAL_FUNCTION(Symbol, Function)                          \
	concrete::Object concrete::internal_symbol::Symbol(                   \
		ContinuationOp op,                                            \
		BlockId &,                                                    \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		return Function(*args, *kwargs);                              \
	}

#define CONCRETE_INTERNAL_CONTINUABLE(Symbol,                                 \
                                      ContinuableType,                        \
                                      ContinuationType)                       \
	concrete::Object concrete::internal_symbol::Symbol(                   \
		ContinuationOp op,                                            \
		BlockId &continuation_id,                                     \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		ContinuableType continuable;                                  \
		return ContinuableCall<ContinuationType>(                     \
			op, continuation_id, continuable, args, kwargs);      \
	}

#define CONCRETE_INTERNAL_CONTINUABLE_ARGS(Symbol,                            \
                                           ContinuableType,                   \
                                           ContinuationType,                  \
                                           ContinuableArgs...)                \
	concrete::Object concrete::internal_symbol::Symbol(                   \
		ContinuationOp op,                                            \
		BlockId &continuation_id,                                     \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		ContinuableType continuable(ContinuableArgs);                 \
		return ContinuableCall<ContinuationType>(                     \
			op, continuation_id, continuable, args, kwargs);      \
	}

#include "internal-inline.hpp"

#endif
