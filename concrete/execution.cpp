/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "execution.hpp"

#include <cassert>

#include <concrete/exception.hpp>
#include <concrete/objects/callable.hpp>
#include <concrete/objects/function.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/none.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/opcodes.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

/*
 * Frame Data
 */

ExecutionFrame::Data::Data(ExecutionFrame parent,
                                 const CodeObject &code,
                                 const DictObject &dict) throw ():
	parent(parent),
	code(code),
	dict(dict)
{
}

ExecutionFrame::Data::~Data() throw ()
{
	Continuation cont = call_continuation;

	if (cont) {
		Trace("call release enter: callable=%u", call_callable->address());

		call_callable->cast<CallableObject>().release(cont);

		Trace("call release leave: callable=%u", call_callable->address());
	}

	for (unsigned int i = stack_pointer; i-- > 0; )
		stack_objects[i].~Portable<Object>();
}

unsigned int ExecutionFrame::Data::stack_size() const throw ()
{
	return (Arena::AllocationSize(this) - sizeof (Data)) / sizeof (Portable<Object>);
}

/*
 * Frame
 */

ExecutionFrame ExecutionFrame::New(ExecutionFrame parent,
                                   const CodeObject &code,
                                   const DictObject &dict)
{
	return NewCustomSizePointer<ExecutionFrame>(
		sizeof (Data) + sizeof (Portable<Object>) * code.stacksize(),
		parent, code, dict);
}

Object ExecutionFrame::result()
{
	Trace("frame result %u", address());

	return pop();
}

void ExecutionFrame::destroy() throw ()
{
	DestroyPointer(*this);
}

ExecutionFrame ExecutionFrame::parent() const
{
	return data()->parent;
}

CodeObject ExecutionFrame::code() const
{
	return data()->code;
}

DictObject ExecutionFrame::dict() const
{
	return data()->dict;
}

template <typename T>
T ExecutionFrame::load_bytecode()
{
	BytesObject bytecode = code().bytecode();
	size_t pos = *data()->bytecode_position;

	if (pos + sizeof (T) > bytecode.size())
		throw RuntimeError("Execution frame ran out of bytecode");

	const uint8_t *ptr = bytecode.c_data() + pos;

	data()->bytecode_position = pos + sizeof (T);

	return *reinterpret_cast<const T *> (ptr);
}

Object ExecutionFrame::call_callable() const
{
	return data()->call_callable;
}

Continuation ExecutionFrame::call_continuation() const
{
	return data()->call_continuation;
}

void ExecutionFrame::set_call(const Object &callable, Continuation cont)
{
	data()->call_callable = callable;
	data()->call_continuation = cont;
}

void ExecutionFrame::push(const Object &object)
{
	unsigned int i = data()->stack_pointer;
	if (i >= data()->stack_size())
		throw RuntimeError("stack overflow");

	new (&data()->stack_objects[i]) Portable<Object>(object);
	data()->stack_pointer = ++i;
}

Object ExecutionFrame::pop()
{
	unsigned int i = data()->stack_pointer;
	if (i == 0)
		throw RuntimeError("stack underflow");

	data()->stack_pointer = --i;
	Object object(data()->stack_objects[i]);
	data()->stack_objects[i].~Portable<Object>();

	return object;
}

ExecutionFrame::Data *ExecutionFrame::data() const
{
	return data_cast<Data>();
}

/*
 * Frame Data
 */

Execution::Data::Data(ExecutionFrame frame) throw ():
	initial(frame),
	current(frame)
{
	Trace("frame new initial %u", initial->address());
}

Execution::Data::~Data() throw ()
{
	Trace("frame destroy initial %u", initial->address());

	initial->destroy();
}

/*
 * Execution
 */

Execution Execution::New(const CodeObject &code)
{
	return NewPointer<Execution>(ExecutionFrame::New(ExecutionFrame(), code, DictObject::New()));
}

void Execution::destroy() throw ()
{
	DestroyPointer(*this);
}

bool Execution::execute()
{
	if (frame())
		if (!resume_call())
			execute_op();

	return bool(frame());
}

ExecutionFrame Execution::new_frame(const CodeObject &code, const DictObject &dict)
{
	ExecutionFrame parent = data()->current;
	ExecutionFrame current = ExecutionFrame::New(parent, code, dict);

	data()->current = current;

	return current;
}

Execution::Data *Execution::data() const
{
	return data_cast<Data>();
}

ExecutionFrame Execution::frame() const
{
	return data()->current;
}

CodeObject Execution::code() const
{
	return frame().code();
}

DictObject Execution::dict() const
{
	return frame().dict();
}

template <typename T>
T Execution::load()
{
	return frame().load_bytecode<T>();
}

void Execution::push(const Object &object)
{
	frame().push(object);
}

Object Execution::pop()
{
	return frame().pop();
}

void Execution::initiate_call(const Object &callable)
{
	initiate_call(callable, TupleObject::New());
}

void Execution::initiate_call(const Object &callable, const TupleObject &args)
{
	initiate_call(callable, args, DictObject::NewEmpty());
}

void Execution::initiate_call(const Object &callable,
                              const TupleObject &args,
                              const DictObject &kwargs)
{
	ExecutionFrame current = frame();
	Continuation cont;

	Trace("call init enter: callable=%u frame=%u",
	      callable.address(), current.address());

	auto result = callable.require<CallableObject>().initiate(cont, args, kwargs);

	Trace("call init leave: callable=%u frame=%u done=%s return=%u",
	      callable.address(), current.address(),
	      (cont ? "false" : "true"), result.address());

	if (cont)
		current.set_call(callable, cont);
	else
		current.push(result);
}

bool Execution::resume_call()
{
	ExecutionFrame current = frame();
	Continuation cont = current.call_continuation();

	if (!cont)
		return false;

	Object callable = current.call_callable();

	Trace("call resume enter: callable=%u frame=%u",
	      callable.address(), current.address());

	auto result = callable.cast<CallableObject>().resume(cont);

	Trace("call resume leave: callable=%u frame=%u done=%s return=%d",
	      callable.address(), current.address(),
	      (cont ? "false" : "true"), result.address());

	if (!cont) {
		callable = Object();
		current.push(result);
	}

	current.set_call(callable, cont);

	return true;
}

void Execution::execute_op()
{
	using namespace opcodes;

	unsigned int op = load<uint8_t>();

	Trace("opcode %3u", op);

	switch (Opcode(op)) {
	case PopTop:              op_pop_top(); break;
	case Nop:                 break;
	case BinaryAdd:           op_binary_add(); break;
	case ReturnValue:         op_return_value(); break;
	case StoreName:           op_store_name(); break;
	case LoadConst:           op_load_const(); break;
	case LoadName:            op_load_name(); break;
	case LoadAttr:            op_load_attr(); break;
	case ImportName:          op_import_name(); break;
	case ImportFrom:          op_import_from(); break;
	case LoadFast:            op_load_fast(); break;
	case CallFunction:        op_call_function(); break;
	case MakeFunction:        op_make_function(); break;

	default:
		Trace("unsupported opcode: %u", op);
		throw RuntimeError("unsupported opcode");
	}
}

void Execution::op_pop_top()
{
	pop();
}

void Execution::op_binary_add()
{
	auto b = pop();
	auto a = pop();
	initiate_call(a.protocol()->add, TupleObject::New(a, b));
}

void Execution::op_return_value()
{
	ExecutionFrame current = frame();

	Trace("frame exit %u", current.address());

	data()->current = current.parent();
}

void Execution::op_store_name()
{
	auto name = code().names().get_item(load<uint16_t>());
	auto value = pop();
	dict().set_item(name, value);
}

void Execution::op_load_const()
{
	push(code().consts().get_item(load<uint16_t>()));
}

void Execution::op_load_name()
{
	auto name = code().names().get_item(load<uint16_t>());
	Object value;

	if (!dict().get_item(name, value))
		value = Context::Active().load_builtin_name(name);

	push(value);
}

void Execution::op_load_attr()
{
	auto object = pop();

	// TODO: support all object types
	auto dict = object.require<ModuleObject>().dict();

	auto name = code().names().get_item(load<uint16_t>());
	push(dict.get_item(name));
}

void Execution::op_import_name()
{
	auto from = pop();
	auto level = pop().require<LongObject>();
	auto name = code().names().get_item(load<uint16_t>());
	auto module = Context::Active().import_builtin_module(name);

	if (!from.check<NoneObject>()) {
		auto fromlist = from.require<TupleObject>();
		assert(fromlist.size() == 1);
		push(module.cast<ModuleObject>().dict().get_item(fromlist.get_item(0)));
	}

	push(module);
}

void Execution::op_import_from()
{
	auto module = pop().require<ModuleObject>();
	auto name = code().names().get_item(load<uint16_t>());
	push(module.dict().get_item(name));
}

void Execution::op_load_fast()
{
	auto varname = code().varnames().get_item(load<uint16_t>());
	push(dict().get_item(varname));
}

void Execution::op_call_function()
{
	unsigned int argc = load<uint8_t>();
	unsigned int kwargc = load<uint8_t>();

	auto args = TupleObject::NewWithSize(argc);
	auto kwargs = DictObject::NewWithCapacity(kwargc);

	for (unsigned int i = 0; i < kwargc; i++) {
		auto value = pop();
		auto key = pop();

		kwargs.set_item(key, value);
	}

	for (unsigned int i = argc; i-- > 0; )
		args.init_item(i, pop());

	initiate_call(pop(), args, kwargs);
}

void Execution::op_make_function()
{
	if (load<uint16_t>() > 0)
		throw RuntimeError("default function arguments not supported");

	auto code = pop().require<CodeObject>();
	push(FunctionObject::New(code));
}

} // namespace
