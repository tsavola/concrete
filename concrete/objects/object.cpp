/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "object-content.hpp"

#include <boost/format.hpp>

#include <concrete/context.hpp>
#include <concrete/objects/bytes-content.hpp>
#include <concrete/objects/callable-content.hpp>
#include <concrete/objects/code-content.hpp>
#include <concrete/objects/dict-content.hpp>
#include <concrete/objects/function-content.hpp>
#include <concrete/objects/internal-content.hpp>
#include <concrete/objects/long-content.hpp>
#include <concrete/objects/module-content.hpp>
#include <concrete/objects/none-content.hpp>
#include <concrete/objects/string-content.hpp>
#include <concrete/objects/tuple-content.hpp>
#include <concrete/objects/type-content.hpp>
#include <concrete/util/nested.hpp>

namespace concrete {

void ObjectTypeInit(const TypeObject &type)
{
	type.init_builtin(StringObject::New("object"));

	type.protocol().repr  = InternalObject::New(internal::ObjectType_Repr);
	type.protocol().str   = InternalObject::New(internal::ObjectType_Str);
}

Object::Content::Content(BlockId type_id, NoRefcountInit no_refcount_init):
	type(type_id, PortableRawInit())
{
}

Object::Content::Content(const TypeObject &type):
	refcount(0),
	type(type)
{
}

Object::Protocol::Protocol()
{
}

Object::Protocol::Protocol(const NoneObject &none) throw ():
	add(none),
	repr(none),
	str(none)
{
}

BlockId Object::RawAccess::Default() throw ()
{
	return Context::SystemObjects()->none.id();
}

void Object::RawAccess::Ref(BlockId id) throw ()
{
	auto content = Context::NonthrowingBlockPointer<Object::Content>(id);
	if (content)
		content->refcount = int(content->refcount) + 1;
}

void Object::RawAccess::Unref(BlockId id) throw ()
{
	auto content = Context::NonthrowingBlockPointer<Object::Content>(id);
	if (content) {
		int refcount = int(content->refcount) - 1;
		content->refcount = refcount;
		assert(refcount >= 0);
		if (refcount == 0)
			Destroy(content, id);
	}
}

TypeObject Object::Type()
{
	return Context::SystemObjects()->object_type;
}

Object Object::New()
{
	return Object(Context::NewBlock<Content>(Type()));
}

void Object::Destroy(Content *content, BlockId id) throw ()
{
	auto builtin = Context::NonthrowingSystemObjects();
	if (builtin == NULL)
		return;

	auto type = content->type->cast<TypeObject>();

	if (type == builtin->object_type)
		static_cast<Object::Content *> (content)->~Content();
	else if (type == builtin->type_type)
		static_cast<TypeObject::Content *> (content)->~Content();
	else if (type == builtin->long_type)
		static_cast<LongObject::Content *> (content)->~Content();
	else if (type == builtin->bytes_type)
		static_cast<BytesObject::Content *> (content)->~Content();
	else if (type == builtin->string_type)
		static_cast<StringObject::Content *> (content)->~Content();
	else if (type == builtin->tuple_type)
		static_cast<TupleObject::Content *> (content)->~Content();
	else if (type == builtin->dict_type)
		static_cast<DictObject::Content *> (content)->~Content();
	else if (type == builtin->code_type)
		static_cast<CodeObject::Content *> (content)->~Content();
	else if (type == builtin->function_type)
		static_cast<FunctionObject::Content *> (content)->~Content();
	else if (type == builtin->internal_type)
		static_cast<InternalObject::Content *> (content)->~Content();
	else if (type == builtin->module_type)
		static_cast<ModuleObject::Content *> (content)->~Content();
	else {
		assert(false);
		return;
	}

	Context::FreeBlock(id);
}

Object::Object():
	m_id(RawAccess::Default())
{
	ref();
}

Object::Object(BlockId id) throw ():
	m_id(id)
{
	ref();
}

Object::Object(const Object &other) throw ():
	m_id(other.m_id)
{
	ref();
}

Object::~Object() throw ()
{
	unref();
}

Object &Object::operator=(const Object &other) throw ()
{
	unref();
	m_id = other.m_id;
	ref();
	return *this;
}

bool Object::operator==(const Object &other) const throw ()
{
	return m_id == other.m_id;
}

bool Object::operator!=(const Object &other) const throw ()
{
	return m_id != other.m_id;
}

BlockId Object::id() const throw ()
{
	return m_id;
}

TypeObject Object::type() const
{
	// don't use cast<TypeObject>() because it causes an infinite recursion
	return TypeObject(content()->type.id());
}

const Object::Protocol &Object::protocol() const
{
	return type().protocol();
}

void Object::ref() const throw ()
{
	RawAccess::Ref(id());
}

void Object::unref() const throw ()
{
	RawAccess::Unref(id());
}

Object::Content *Object::content() const
{
	return content_pointer<Content>();
}

Object Object::add(const Object &other) const
{
	return protocol().add->require<InternalObject>().call(*this, other);
}

StringObject Object::repr() const
{
	return protocol().repr->require<InternalObject>().call(*this).require<StringObject>();
}

StringObject Object::str() const
{
	return protocol().str->require<InternalObject>().call(*this).require<StringObject>();
}

static Object ObjectRepr(const TupleObject &args, const DictObject &kwargs)
{
	auto self = args.get_item(0);

	return StringObject::New(
		(boost::format("<%s object at 0x%lx>")
		 % self.type().name().data()
		 % self.id()).str());
}

struct NestedCall ObjectStr(const TupleObject &args, const DictObject &kwargs)
{
	return NestedCall(args.get_item(0).protocol().repr, args, kwargs);
}

} // namespace

CONCRETE_INTERNAL_FUNCTION   (ObjectType_Repr, ObjectRepr)
CONCRETE_INTERNAL_NESTED_CALL(ObjectType_Str,  ObjectStr)
