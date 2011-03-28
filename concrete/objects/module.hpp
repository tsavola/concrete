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

	ModuleBlock(const TypeObject &type, const DictObject &dict):
		ObjectBlock(type),
		dict(dict)
	{
	}
} CONCRETE_PACKED;

template <typename Ops>
class ModuleLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::SystemObjects()->module_type;
	}

	static ModuleLogic New(const DictObject &dict)
	{
		return Context::NewBlock<ModuleBlock>(Type(), dict);
	}

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	ModuleLogic(const ModuleLogic<OtherOps> &other) throw ():
		ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	ModuleLogic &operator=(const ModuleLogic<OtherOps> &other) throw ()
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	DictObject dict() const
	{
		return module_block()->dict;
	}

protected:
	ModuleLogic(BlockId id) throw ():
		ObjectLogic<Ops>(id)
	{
	}

	ModuleBlock *module_block() const
	{
		return static_cast<ModuleBlock *> (ObjectLogic<Ops>::object_block());
	}
} CONCRETE_PACKED;

typedef ModuleLogic<ObjectOps>         ModuleObject;
typedef ModuleLogic<PortableObjectOps> PortableModuleObject;

void ModuleTypeInit(const TypeObject &type);

} // namespace

#endif
