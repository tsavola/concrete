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

namespace internal {
	enum SymbolId {
#		define CONCRETE_INTERNAL_SYMBOL(Symbol)  Symbol,
#		include <concrete/internals.hpp>
#		undef CONCRETE_INTERNAL_SYMBOL
	};
}

class InternalObject: public CallableObject {
	friend class Pointer;
	friend class Object;

public:
	typedef Object Function(Continuation &cont,
                                Continuation::Stage stage,
                                const TupleObject *args,
                                const DictObject *kwargs);

	static TypeObject Type();
	static InternalObject New(internal::SymbolId symbol_id);

	InternalObject(const InternalObject &other) throw (): CallableObject(other) {}

	template <typename... Args> Object immediate_call(Args... args) const;

	Object continuable_call(Continuation &cont,
	                        Continuation::Stage stage,
	                        const TupleObject *args = NULL,
	                        const DictObject *kwargs = NULL) const;

protected:
	struct Data;

	explicit InternalObject(unsigned int address) throw (): CallableObject(address) {}

private:
	Object immediate_call_args(const TupleObject &args) const;

	Data *data() const;
};

void InternalObjectTypeInit(const TypeObject &type, const char *name = "internal");

namespace internal_symbol {
#	define CONCRETE_INTERNAL_SYMBOL(Symbol)  InternalObject::Function Symbol;
#	include <concrete/internals.hpp>
#	undef CONCRETE_INTERNAL_SYMBOL
}

} // namespace

#define CONCRETE_INTERNAL_FUNCTION(Symbol, Function)                          \
	concrete::Object concrete::internal_symbol::Symbol(                   \
		Continuation &,                                               \
		Continuation::Stage stage,                                    \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		return Function(*args, *kwargs);                              \
	}

#define CONCRETE_INTERNAL_CONTINUATION(Symbol, ImplType)                      \
	concrete::Object concrete::internal_symbol::Symbol(                   \
		Continuation &cont,                                           \
		Continuation::Stage stage,                                    \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		return Continuation::Call<ImplType>(                          \
			cont, stage, args, kwargs);                           \
	}

#define CONCRETE_INTERNAL_CONTINUATION_PARAMS(Symbol, ImplType, ImplParams...)\
	concrete::Object concrete::internal_symbol::Symbol(                   \
		Continuation &cont,                                           \
		Continuation::Stage stage,                                    \
		const TupleObject *args,                                      \
		const DictObject *kwargs)                                     \
	{                                                                     \
		return Continuation::Call<ImplType>(                          \
			cont, stage, args, kwargs, ImplParams);               \
	}

#include "internal-inline.hpp"

#endif
