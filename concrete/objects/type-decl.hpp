/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_TYPE_DECL_HPP
#define CONCRETE_OBJECTS_TYPE_DECL_HPP

#include "type-fwd.hpp"

#include <concrete/objects/none-fwd.hpp>
#include <concrete/objects/object-decl.hpp>
#include <concrete/objects/string-fwd.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct TypeBlock;

template <typename Ops>
class TypeLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;
	friend struct ObjectBlock;

public:
	static TypeObject Type();

	static TypeLogic NewBuiltin(const NoneObject &none);
	static TypeLogic NewBuiltin(const NoneObject &none, const TypeObject &type);
	static TypeLogic New(const StringObject &name);

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	TypeLogic(const TypeLogic<OtherOps> &other) throw ():
		ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	TypeLogic &operator=(const TypeLogic<OtherOps> &other) throw ()
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	void init_builtin(const StringObject &name) const;

	StringObject name() const;
	ObjectProtocol &protocol() const;

protected:
	TypeLogic(BlockId id) throw ():
		ObjectLogic<Ops>(id)
	{
	}

	TypeBlock *type_block() const;

} CONCRETE_PACKED;

template <typename T>
struct TypeCheck {
	bool operator()(const TypeObject &type)
	{
		return type == T::Type();
	}
};

} // namespace

#endif
