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
#include <concrete/objects.hpp>
#include <concrete/util/nested.hpp>

namespace concrete {

Object ObjectProtocol::Add(const Object &self, const Object &other)
{
	return self.protocol().add.require<InternalObject>().call(self, other);
}

Object ObjectProtocol::Repr(const Object &self)
{
	return self.protocol().repr.require<InternalObject>().call(self);
}

Object ObjectProtocol::Str(const Object &self)
{
	return self.protocol().str.require<InternalObject>().call(self);
}

CONCRETE_INTERNAL(Object_repr)(const TupleObject &args, const DictObject &kwargs)
{
	auto self = args.get_item(0);

	return StringObject::New(
		(boost::format("<%s object at 0x%lx>") % self.type().name().data() % self.id()).str());
}

CONCRETE_INTERNAL_NESTED_CALL(Object_str)(const TupleObject &args, const DictObject &kwargs)
{
	return NestedCall(args.get_item(0).protocol().repr, args, kwargs);
}

void ObjectInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("object"));

	type.protocol().repr  = InternalObject::New(internals::Object_repr);
	type.protocol().str   = InternalObject::New(internals::Object_str);
}

template <typename T>
static void destruct(ObjectBlock *block)
{
	static_cast<T *> (block)->~T();
}

void ObjectDestroy(ObjectBlock *block, BlockId id)
{
	auto type = block->type();
	auto &builtin = Context::BuiltinObjects();

	if (type == builtin.object_type)   { destruct<ObjectBlock>(block);   } else
	if (type == builtin.type_type)     { destruct<TypeBlock>(block);     } else
	if (type == builtin.none_type)     { assert(false); return;          } else
	if (type == builtin.long_type)     { destruct<LongBlock>(block);     } else
	if (type == builtin.bytes_type)    { destruct<BytesBlock>(block);    } else
	if (type == builtin.string_type)   { destruct<StringBlock>(block);   } else
	if (type == builtin.tuple_type)    { destruct<TupleBlock>(block);    } else
	if (type == builtin.dict_type)     { destruct<DictBlock>(block);     } else
	if (type == builtin.code_type)     { destruct<CodeBlock>(block);     } else
	if (type == builtin.function_type) { destruct<FunctionBlock>(block); } else
	if (type == builtin.internal_type) { destruct<InternalBlock>(block); } else
	if (type == builtin.module_type)   { destruct<ModuleBlock>(block);   } else
	                                   { assert(false); return;          }

	Context::Active().arena().free(id);
}

} // namespace
