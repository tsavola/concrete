/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_NONE_DECL_HPP
#define CONCRETE_OBJECTS_NONE_DECL_HPP

#include "none-fwd.hpp"

#include <concrete/objects/object-decl.hpp>
#include <concrete/objects/type-fwd.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

template <typename Ops>
class NoneLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type();
	static NoneLogic NewBuiltin();

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	NoneLogic(const NoneLogic<OtherOps> &other) throw ():
		ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	NoneLogic &operator=(const NoneLogic<OtherOps> &other) throw ()
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	void init_builtin(const TypeObject &type);

protected:
	NoneLogic(BlockId id) throw ():
		ObjectLogic<Ops>(id)
	{
	}
} CONCRETE_PACKED;

} // namespace

#endif
