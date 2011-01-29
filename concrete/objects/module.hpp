/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_MODULE_HPP
#define CONCRETE_OBJECTS_MODULE_HPP

#include <concrete/block.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct ModuleBlock: ObjectBlock {
	const PortableDictObject dict;

	ModuleBlock(const TypeObject &type, const DictObject &dict): ObjectBlock(type), dict(dict)
	{
	}

} CONCRETE_PACKED;

template <typename Ops>
class module_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().module_type;
	}

	static module_object New(const DictObject &dict) throw (AllocError)
	{
		auto id = Context::Alloc(sizeof (ModuleBlock));
		new (Context::Pointer(id)) ModuleBlock(Type(), dict);
		return id;
	}

	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	module_object(const module_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	module_object &operator=(const module_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
		return *this;
	}

	DictObject dict() const
	{
		return module_block()->dict;
	}

protected:
	module_object(BlockId id): object<Ops>(id)
	{
	}

	ModuleBlock *module_block() const
	{
		return static_cast<ModuleBlock *> (object<Ops>::object_block());
	}

} CONCRETE_PACKED;

typedef module_object<ObjectOps>         ModuleObject;
typedef module_object<PortableObjectOps> PortableModuleObject;

} // namespace

#endif
