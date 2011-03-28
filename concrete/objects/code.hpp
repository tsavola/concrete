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

template <typename Ops> class CodeLogic;

typedef CodeLogic<ObjectOps>         CodeObject;
typedef CodeLogic<PortableObjectOps> PortableCodeObject;

struct CodeBlock: ObjectBlock {
	Portable<uint32_t> stacksize;
	PortableBytesObject code;
	PortableTupleObject consts;
	PortableTupleObject names;
	PortableTupleObject varnames;

	CodeBlock(const TypeObject &type,
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

	static CodeObject Load(const void *data, size_t size);

} CONCRETE_PACKED;

template <typename Ops>
class CodeLogic: public ObjectLogic<Ops> {
	friend class ObjectLogic<ObjectOps>;
	friend class ObjectLogic<PortableObjectOps>;

public:
	static TypeObject Type()
	{
		return Context::SystemObjects()->code_type;
	}

	static CodeLogic New(unsigned int stacksize,
	                     const BytesObject &code,
	                     const TupleObject &consts,
	                     const TupleObject &names,
	                     const TupleObject &varnames)
	{
		return Context::NewBlock<CodeBlock>(Type(), stacksize, code, consts, names, varnames);
	}

	static CodeLogic Load(const void *data, size_t size)
	{
		return CodeBlock::Load(data, size);
	}

	using ObjectLogic<Ops>::operator==;
	using ObjectLogic<Ops>::operator!=;

	template <typename OtherOps>
	CodeLogic(const CodeLogic<OtherOps> &other) throw ():
		ObjectLogic<Ops>(other)
	{
	}

	template <typename OtherOps>
	CodeLogic &operator=(const CodeLogic<OtherOps> &other) throw ()
	{
		ObjectLogic<Ops>::operator=(other);
		return *this;
	}

	unsigned int stacksize() const { return code_block()->stacksize; }
	BytesObject code() const       { return code_block()->code; }
	TupleObject consts() const     { return code_block()->consts; }
	TupleObject names() const      { return code_block()->names; }
	TupleObject varnames() const   { return code_block()->varnames; }

protected:
	CodeLogic(BlockId id) throw ():
		ObjectLogic<Ops>(id)
	{
	}

	CodeBlock *code_block() const
	{
		return static_cast<CodeBlock *> (ObjectLogic<Ops>::object_block());
	}
} CONCRETE_PACKED;

void CodeTypeInit(const TypeObject &type);

} // namespace

#endif
