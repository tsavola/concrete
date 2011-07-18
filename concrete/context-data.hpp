/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_CONTEXT_DATA_HPP
#define CONCRETE_CONTEXT_DATA_HPP

#include "context.hpp"

#include <concrete/arena.hpp>
#include <concrete/execution.hpp>
#include <concrete/list.hpp>
#include <concrete/objects/object-data.hpp>
#include <concrete/portable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct Context::Data: Noncopyable {
	Data(const NoneObject &none,
	     const TypeObject &type_type,
	     const TypeObject &object_type,
	     const TypeObject &none_type,
	     const TypeObject &string_type,
	     const TypeObject &long_type,
	     const TypeObject &bool_type,
	     const TypeObject &bytes_type,
	     const TypeObject &tuple_type,
	     const TypeObject &dict_type,
	     const TypeObject &code_type,
	     const TypeObject &function_type,
	     const TypeObject &internal_type,
	     const TypeObject &module_type) throw ();

	const Portable<NoneObject> none;
	const Portable<TypeObject> type_type;
	const Portable<TypeObject> object_type;
	const Portable<TypeObject> none_type;
	const Portable<TypeObject> string_type;
	const Portable<TypeObject> long_type;
	const Portable<TypeObject> bool_type;
	Portable<Object>           bool_false;
	Portable<Object>           bool_true;
	const Portable<TypeObject> bytes_type;
	const Portable<TypeObject> tuple_type;
	Portable<Object>           tuple_empty;
	const Portable<TypeObject> dict_type;
	const Portable<TypeObject> code_type;
	const Portable<TypeObject> function_type;
	const Portable<TypeObject> internal_type;
	const Portable<TypeObject> module_type;

	Portable<Object>           builtins;
	Portable<Object>           modules;

	PortableList<Execution>    runnable;
	PortableList<Execution>    waiting;
	Portable<Execution>        current;
} CONCRETE_PACKED;

} // namespace

#endif
