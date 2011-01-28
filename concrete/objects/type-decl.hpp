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
class type_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;
	friend struct ObjectBlock;

public:
	static TypeObject Type();

	static type_object NewBuiltin(const NoneObject &none)
		throw (AllocError);

	static type_object NewBuiltin(const TypeObject &type, const NoneObject &none)
		throw (AllocError);

	static type_object New(const StringObject &name)
		throw (AllocError);

	using object<Ops>::operator==;
	using object<Ops>::operator!=;
	using object<Ops>::id;

	template <typename OtherOps>
	type_object(const type_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	type_object &operator=(const type_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
		return *this;
	}

	void init_builtin(const StringObject &name);

	StringObject name() const;

protected:
	type_object(BlockId id): object<Ops>(id)
	{
	}

	TypeBlock *type_block() const;

} CONCRETE_PACKED;

} // namespace

#endif
