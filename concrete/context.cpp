/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "context.hpp"

#include <concrete/modules/builtins.hpp>
#include <concrete/modules/concrete.hpp>
#include <concrete/objects.hpp>

namespace concrete {

Context::Context()
{
	ContextScope scope(*this);

	auto none = NoneObject::NewBuiltin();
	m_builtin_none = NewBlock<BuiltinNoneBlock>(none);

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

	m_builtin_objects = NewBlock<BuiltinObjectsBlock>(
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

	none.init_builtin(none_type);

	TypeTypeInit(type_type);
	ObjectTypeInit(object_type);
	NoneTypeInit(none_type);
	StringTypeInit(string_type);
	LongTypeInit(long_type);
	BytesTypeInit(bytes_type);
	TupleTypeInit(tuple_type);
	DictTypeInit(dict_type);
	CodeTypeInit(code_type);
	FunctionTypeInit(function_type);
	InternalTypeInit(internal_type);
	ModuleTypeInit(module_type);

	auto modules = DictObject::New();
	auto builtins = BuiltinsModuleInit(modules);

	ConcreteModuleInit(modules);

	builtin_objects().builtins = builtins;
	builtin_objects().modules = modules;
}

Object Context::LoadBuiltinName(const Object &name)
{
	return BuiltinObjects().builtins.cast<DictObject>().get_item(name);
}

Object Context::ImportBuiltinModule(const Object &name)
{
	return BuiltinObjects().modules.cast<DictObject>().get_item(name);
}

} // namespace
