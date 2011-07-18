/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "object-data.hpp"

#include <boost/format.hpp>

#include <concrete/context-data.hpp>
#include <concrete/nested-data.hpp>
#include <concrete/objects/bytes-data.hpp>
#include <concrete/objects/code-data.hpp>
#include <concrete/objects/dict-data.hpp>
#include <concrete/objects/function-data.hpp>
#include <concrete/objects/internal-data.hpp>
#include <concrete/objects/long-data.hpp>
#include <concrete/objects/module-data.hpp>
#include <concrete/objects/none-data.hpp>
#include <concrete/objects/string-data.hpp>
#include <concrete/objects/tuple-data.hpp>
#include <concrete/objects/type-data.hpp>
#include <concrete/util/assert.hpp>

namespace concrete {

Object::Data::Data(const Pointer &type_pointer) throw ():
	m_type_pointer(Referenced(type_pointer))
{
}

Object::Data::Data(const TypeObject &type) throw ():
	m_type_pointer(Referenced(type))
{
}

Object::Data::~Data() throw ()
{
	RawAccess::Unref(m_type_pointer->address());
}

void Object::Data::init_none_type(const Portable<TypeObject> &none_type) throw ()
{
	m_type_pointer = *none_type;
	RawAccess::Ref(m_type_pointer->address());
}

void Object::Data::ref() throw ()
{
	m_refcount = int(m_refcount) + 1;
}

void Object::Data::unref(unsigned int address) throw ()
{
	int refcount = int(m_refcount) - 1;
	m_refcount = refcount;
	assert(refcount >= 0);
	if (refcount == 0)
		Destroy(address, this);
}

TypeObject Object::Data::type() const throw ()
{
	return m_type_pointer->cast<TypeObject>();
}

Object::RawAccess::RawType Object::RawAccess::InitRef() throw ()
{
	return Referenced(Context::Active().none_pointer()).address();
}

void Object::RawAccess::Ref(RawType address) throw ()
{
	auto data = NonthrowingDataCast<Data>(address);
	if (data)
		data->ref();
}

void Object::RawAccess::Unref(RawType address) throw ()
{
	auto data = NonthrowingDataCast<Data>(address);
	if (data)
		data->unref(address);
}

Object::RawAccess::RawType Object::RawAccess::ExtractRef(const Pointer &object) throw ()
{
	return Referenced(object).address();
}

TypeObject Object::Type()
{
	return Context::Active().data()->object_type;
}

Object Object::New()
{
	return NewObject<Object>();
}

const Pointer &Object::Referenced(const Pointer &object) throw ()
{
	auto data = object.nonthrowing_data_cast<Data>();
	if (data)
		data->ref();

	return object;
}

void Object::Destroy(unsigned int address, Data *data) throw ()
{
	auto context = Context::Active().nonthrowing_data();
	if (context == NULL)
		return;

	auto type = data->type();

	if (type == context->object_type)
		TypedDestroy<Object>(address, data);
	else if (type == context->type_type)
		TypedDestroy<TypeObject>(address, data);
	else if (type == context->long_type)
		TypedDestroy<LongObject>(address, data);
	else if (type == context->bytes_type)
		TypedDestroy<BytesObject>(address, data);
	else if (type == context->string_type)
		TypedDestroy<StringObject>(address, data);
	else if (type == context->tuple_type)
		TypedDestroy<TupleObject>(address, data);
	else if (type == context->dict_type)
		TypedDestroy<DictObject>(address, data);
	else if (type == context->code_type)
		TypedDestroy<CodeObject>(address, data);
	else if (type == context->function_type)
		TypedDestroy<FunctionObject>(address, data);
	else if (type == context->internal_type)
		TypedDestroy<InternalObject>(address, data);
	else if (type == context->module_type)
		TypedDestroy<ModuleObject>(address, data);
	else
		assert(false);
}

template <typename ObjectType>
void Object::TypedDestroy(unsigned int address, Data *data) throw ()
{
	DestroyData(address, static_cast<typename ObjectType::Data *> (data));
}

Object::Object() throw ():
	Pointer(Referenced(Context::Active().none_pointer()))
{
}

Object::Object(const Object &other) throw ():
	Pointer(Referenced(other))
{
}

Object::Object(unsigned int address) throw ():
	Pointer(address)
{
	ref();
}

Object::~Object() throw ()
{
	unref();
}

void Object::operator=(const Object &other) throw ()
{
	unref();
	Pointer::operator=(Referenced(other));
}

Object::operator bool() const throw ()
{
	return operator!=(Context::Active().none_pointer());
}

TypeObject Object::type() const
{
	return data()->type();
}

const PortableObjectProtocol *Object::protocol() const
{
	return type().protocol();
}

void Object::ref() const throw ()
{
	auto data = nonthrowing_data();
	if (data)
		data->ref();
}

void Object::unref() const throw ()
{
	auto data = nonthrowing_data();
	if (data)
		data->unref(m_address);
}

Object::Data *Object::data() const
{
	return data_cast<Data>();
}

Object::Data *Object::nonthrowing_data() const throw ()
{
	return nonthrowing_data_cast<Data>();
}

StringObject Object::repr() const
{
	return protocol()->repr->require<InternalObject>().immediate_call(*this).require<StringObject>();
}

StringObject Object::str() const
{
	return protocol()->str->require<InternalObject>().immediate_call(*this).require<StringObject>();
}
 
void ObjectTypeInit(const TypeObject &type, const char *name)
{
	type.init_builtin(StringObject::New(name));

	type.protocol()->repr  = InternalObject::New(internal::ObjectType_Repr);
	type.protocol()->str   = InternalObject::New(internal::ObjectType_Str);
}

static Object ObjectRepr(const TupleObject &args, const DictObject &kwargs)
{
	auto self = args.get_item(0);

	return StringObject::New(
		(boost::format("<%s object at 0x%lx>")
		 % self.type().name().c_str()
		 % self.address()).str());
}

static struct NestedCall ObjectStr(const TupleObject &args, const DictObject &kwargs)
{
	return NestedCall(args.get_item(0).protocol()->repr, args, kwargs);
}

} // namespace

CONCRETE_INTERNAL_FUNCTION   (ObjectType_Repr, ObjectRepr)
CONCRETE_INTERNAL_NESTED_CALL(ObjectType_Str,  ObjectStr)
