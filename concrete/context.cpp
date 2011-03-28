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

Context::SystemObjectsBlock::SystemObjectsBlock(const NoneObject &none) throw ():
	none                    (none),
	type_type               (TypeObject::NewBuiltin(none)),
	object_type             (TypeObject::NewBuiltin(none, type_type)),
	none_type               (TypeObject::NewBuiltin(none, type_type)),
	string_type             (TypeObject::NewBuiltin(none, type_type)),
	long_type               (TypeObject::NewBuiltin(none, type_type)),
	bytes_type              (TypeObject::NewBuiltin(none, type_type)),
	tuple_type              (TypeObject::NewBuiltin(none, type_type)),
	tuple_empty             (none),
	dict_type               (TypeObject::NewBuiltin(none, type_type)),
	dict_empty              (none),
	code_type               (TypeObject::NewBuiltin(none, type_type)),
	function_type           (TypeObject::NewBuiltin(none, type_type)),
	internal_type           (TypeObject::NewBuiltin(none, type_type)),
	module_type             (TypeObject::NewBuiltin(none, type_type)),
	builtins                (none),
	modules                 (none)
{
}

Context::Context()
{
	ContextScope scope(*this);

	auto none = NoneObject::NewBuiltin();

	m_system_objects = m_arena.new_block<SystemObjectsBlock>(none);

	none.init_builtin       (system_objects()->none_type);
	TypeTypeInit            (system_objects()->type_type);
	ObjectTypeInit          (system_objects()->object_type);
	NoneTypeInit            (system_objects()->none_type);
	StringTypeInit          (system_objects()->string_type);
	LongTypeInit            (system_objects()->long_type);
	BytesTypeInit           (system_objects()->bytes_type);
	TupleTypeInit           (system_objects()->tuple_type);
	DictTypeInit            (system_objects()->dict_type);
	CodeTypeInit            (system_objects()->code_type);
	FunctionTypeInit        (system_objects()->function_type);
	InternalTypeInit        (system_objects()->internal_type);
	ModuleTypeInit          (system_objects()->module_type);

	auto modules = DictObject::New();

	auto builtins = BuiltinsModuleInit(modules);
	ConcreteModuleInit(modules);

	system_objects()->builtins = builtins;
	system_objects()->modules = modules;
}

Object Context::LoadBuiltinName(const Object &name)
{
	return SystemObjects()->builtins.cast<DictObject>().get_item(name);
}

Object Context::ImportBuiltinModule(const Object &name)
{
	return SystemObjects()->modules.cast<DictObject>().get_item(name);
}

} // namespace
