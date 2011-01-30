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
class none_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type();
	static none_object NewBuiltin() throw (AllocError);

	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	none_object(const none_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	none_object &operator=(const none_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
		return *this;
	}

	void init_builtin(const TypeObject &type);

protected:
	none_object(BlockId id): object<Ops>(id)
	{
	}

} CONCRETE_PACKED;

} // namespace

#endif
