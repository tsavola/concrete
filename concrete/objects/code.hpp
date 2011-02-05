/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_CODE_HPP
#define CONCRETE_OBJECTS_CODE_HPP

#include <cstddef>

#include <concrete/block.hpp>
#include <concrete/objects/bytes.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

struct CodeBlock: ObjectBlock {
	portable<uint32_t> stacksize;
	PortableBytesObject code;
	PortableTupleObject consts;
	PortableTupleObject names;
	PortableTupleObject varnames;

	CodeBlock(
		const TypeObject &type,
		unsigned int stacksize,
		const BytesObject &code,
		const TupleObject &consts,
		const TupleObject &names,
		const TupleObject &varnames):
		ObjectBlock(type),
		stacksize(stacksize),
		code(code),
		consts(consts),
		names(names),
		varnames(varnames)
	{
	}

	static CodeObject Load(const void *data, size_t size) throw (AllocError);

} CONCRETE_PACKED;

template <typename Ops>
class code_object: public object<Ops> {
	friend class object<ObjectOps>;
	friend class object<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::Builtins().code_type;
	}

	static code_object New(
		unsigned int stacksize,
		const BytesObject &code,
		const TupleObject &consts,
		const TupleObject &names,
		const TupleObject &varnames) throw (AllocError)
	{
		auto id = Context::Alloc(sizeof (CodeBlock));
		new (Context::Pointer(id)) CodeBlock(Type(), stacksize, code, consts, names, varnames);
		return id;
	}

	static code_object Load(const void *data, size_t size) throw (AllocError)
	{
		return CodeBlock::Load(data, size);
	}

	using object<Ops>::operator==;
	using object<Ops>::operator!=;

	template <typename OtherOps>
	code_object(const code_object<OtherOps> &other): object<Ops>(other)
	{
	}

	template <typename OtherOps>
	code_object &operator=(const code_object<OtherOps> &other)
	{
		object<Ops>::operator=(other);
		return *this;
	}

	unsigned int stacksize() const { return code_block()->stacksize; }
	BytesObject code() const       { return code_block()->code; }
	TupleObject consts() const     { return code_block()->consts; }
	TupleObject names() const      { return code_block()->names; }
	TupleObject varnames() const   { return code_block()->varnames; }

protected:
	code_object(BlockId id): object<Ops>(id)
	{
	}

	CodeBlock *code_block() const
	{
		return static_cast<CodeBlock *> (object<Ops>::object_block());
	}
} CONCRETE_PACKED;

typedef code_object<ObjectOps>         CodeObject;
typedef code_object<PortableObjectOps> PortableCodeObject;

} // namespace

#endif
