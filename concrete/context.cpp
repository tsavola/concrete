/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "context.hpp"

#include <concrete/modules/concrete.hpp>
#include <concrete/objects/bytes.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/function.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/none.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

void Context::Init()
{
	LongType::RegisterInternals();

	ConcreteModule::RegisterInternals();
}

Context::Context() throw (AllocError)
{
	ContextScope scope(*this);

	auto none          = NoneObject::NewBuiltin();

	m_builtin_none = Alloc(sizeof (BuiltinNoneBlock));
	new (Pointer(m_builtin_none)) BuiltinNoneBlock(none);

	auto type_type     = TypeObject::NewBuiltin();
	auto object_type   = TypeObject::NewBuiltin(type_type);
	auto none_type     = TypeObject::NewBuiltin(type_type);
	auto string_type   = TypeObject::NewBuiltin(type_type);
	auto long_type     = TypeObject::NewBuiltin(type_type);
	auto bytes_type    = TypeObject::NewBuiltin(type_type);
	auto tuple_type    = TypeObject::NewBuiltin(type_type);
	auto dict_type     = TypeObject::NewBuiltin(type_type);
	auto code_type     = TypeObject::NewBuiltin(type_type);
	auto function_type = TypeObject::NewBuiltin(type_type);
	auto internal_type = TypeObject::NewBuiltin(type_type);
	auto module_type   = TypeObject::NewBuiltin(type_type);

	m_builtins = Alloc(sizeof (BuiltinsBlock));
	new (Pointer(m_builtins)) BuiltinsBlock(
		type_type,
		object_type,
		none_type,
		string_type,
		long_type,
		bytes_type,
		tuple_type,
		dict_type,
		code_type,
		function_type,
		internal_type,
		module_type);

	none         .init_builtin(none_type);

	type_type    .init_builtin(StringObject::New("type"));
	object_type  .init_builtin(StringObject::New("object"));
	none_type    .init_builtin(StringObject::New("none"));
	string_type  .init_builtin(StringObject::New("string"));
	LongType::Init(long_type);
	bytes_type   .init_builtin(StringObject::New("bytes"));
	tuple_type   .init_builtin(StringObject::New("tuple"));
	dict_type    .init_builtin(StringObject::New("dict"));
	code_type    .init_builtin(StringObject::New("code"));
	function_type.init_builtin(StringObject::New("function"));
	internal_type.init_builtin(StringObject::New("internal"));
	module_type  .init_builtin(StringObject::New("module"));

	auto modules = DictObject::New(1);

	ConcreteModule::Init(modules);

	builtins().modules = modules;
}

Object Context::ImportBuiltin(const Object &name)
{
	return Builtins().modules.cast<DictObject>().get_item(name);
}

CONCRETE_THREAD_LOCAL Context *Context::m_active;

} // namespace
