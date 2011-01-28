/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "object.hpp"

#include <boost/format.hpp>

#include <concrete/context.hpp>
#include <concrete/objects/bytes.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/function.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/none.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

template <typename T>
static void destruct(ObjectBlock *block)
{
	static_cast<T *> (block)->~T();
}

void ObjectBlock::Destroy(ObjectBlock *block) throw ()
{
	auto type = block->type();
	auto &builtins = Context::Builtins();

	if (type == builtins.object_type)   destruct<ObjectBlock>(block);   else
	if (type == builtins.type_type)     destruct<TypeBlock>(block);     else
	if (type == builtins.none_type)     assert(false);                  else
	if (type == builtins.long_type)     destruct<LongBlock>(block);     else
	if (type == builtins.bytes_type)    destruct<BytesBlock>(block);    else
	if (type == builtins.string_type)   destruct<StringBlock>(block);   else
	if (type == builtins.tuple_type)    destruct<TupleBlock>(block);    else
	if (type == builtins.dict_type)     destruct<DictBlock>(block);     else
	if (type == builtins.code_type)     destruct<CodeBlock>(block);     else
	if (type == builtins.function_type) destruct<FunctionBlock>(block); else
	if (type == builtins.internal_type) destruct<InternalBlock>(block); else
	if (type == builtins.module_type)   destruct<ModuleBlock>(block);   else
	                                    assert(false);

	Context::Free(block);
}

StringObject ObjectBlock::repr(BlockId id) const
{
	auto str = (boost::format("<%s object at 0x%lx>") % type().name().data() % id).str();
	return StringObject::New(str.data(), str.size());
}

} // namespace
