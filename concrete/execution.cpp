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

#include <concrete/byteorder.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/bool.hpp>
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

ExecutionFrame::Data::Data(const ExecutionFrame &parent, const CodeObject &code, const DictObject &dict):
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

ExecutionFrame ExecutionFrame::New(const ExecutionFrame &parent,
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

template <typename T>
T ExecutionFrame::load_bytecode()
{
	BytesObject bytecode = data()->code->bytecode();
	size_t pos = *data()->bytecode_position;

	if (pos + sizeof (T) > bytecode.size())
		throw RuntimeError("Execution frame ran out of bytecode");

	const uint8_t *ptr = bytecode.c_data() + pos;

	data()->bytecode_position = pos + sizeof (T);

	return *reinterpret_cast<const T *> (ptr);
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

Execution::Data::Data(const ExecutionFrame &frame) throw ():
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
	return frame().data()->code;
}

DictObject Execution::dict() const
{
	return frame().data()->dict;
}

void Execution::push(const Object &object)
{
	frame().push(object);
}

Object Execution::pop()
{
	return frame().pop();
}

void Execution::initiate_call(const Object &callable, bool not_filter)
{
	initiate_call(callable, TupleObject::New(), not_filter);
}

void Execution::initiate_call(const Object &callable, const TupleObject &args, bool not_filter)
{
	initiate_call(callable, args, DictObject::NewEmpty(), not_filter);
}

void Execution::initiate_call(const Object &callable,
                              const TupleObject &args,
                              const DictObject &kwargs,
                              bool not_filter)
{
	ExecutionFrame current = frame();
	Continuation cont;

	Trace("call init enter: callable=%u frame=%u",
	      callable.address(), current.address());

	auto result = callable.require<CallableObject>().initiate(cont, args, kwargs);

	Trace("call init leave: callable=%u frame=%u done=%s return=%u",
	      callable.address(), current.address(),
	      (cont ? "false" : "true"), result.address());

	if (cont) {
		current.data()->call_callable = callable;
		current.data()->call_continuation = cont;
		current.data()->call_not_filter = not_filter;
	} else {
		current.push(result);
	}
}

bool Execution::resume_call()
{
	ExecutionFrame current = frame();
	Continuation cont = current.data()->call_continuation;

	if (!cont)
		return false;

	Object callable = current.data()->call_callable;

	Trace("call resume enter: callable=%u frame=%u",
	      callable.address(), current.address());

	auto result = callable.cast<CallableObject>().resume(cont);

	Trace("call resume leave: callable=%u frame=%u done=%s return=%d",
	      callable.address(), current.address(),
	      (cont ? "false" : "true"), result.address());

	if (!cont) {
		if (current.data()->call_not_filter)
			result = BoolObject::FromBool(!result.require<LongObject>().value());

		current.push(result);

		current.data()->call_callable = Object();
		current.data()->call_not_filter = false;
	}

	current.data()->call_continuation = cont;

	return true;
}

void Execution::execute_op()
{
	unsigned int op = frame().load_bytecode<uint8_t>();

	Trace("opcode %3u", op);

	switch (op) {
	case OpPopTop:              op_pop_top(); return;
	case OpNop:                 return;
	case OpBinaryAdd:           op_binary_add(); return;
	case OpReturnValue:         op_return_value(); return;
	}

	unsigned int arg = frame().load_bytecode<uint16_t>();

	switch (op) {
	case OpStoreName:           op_store_name(arg); return;
	case OpLoadConst:           op_load_const(arg); return;
	case OpLoadName:            op_load_name(arg); return;
	case OpLoadAttr:            op_load_attr(arg); return;
	case OpCompareOp:           op_compare_op(arg); return;
	case OpImportName:          op_import_name(arg); return;
	case OpImportFrom:          op_import_from(arg); return;
	case OpLoadFast:            op_load_fast(arg); return;
	case OpStoreFast:           op_store_fast(arg); return;
	case OpCallFunction:        op_call_function(arg); return;
	case OpMakeFunction:        op_make_function(arg); return;
	}

	throw RuntimeError("unsupported opcode");
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

	data()->current = current.data()->parent;
}

void Execution::op_store_name(unsigned int namei)
{
	auto name = code().names().get_item(namei);
	auto value = pop();
	dict().set_item(name, value);
}

void Execution::op_load_const(unsigned int consti)
{
	push(code().consts().get_item(consti));
}

void Execution::op_load_name(unsigned int namei)
{
	auto name = code().names().get_item(namei);
	Object value;

	if (!dict().get_item(name, value))
		value = Context::Active().load_builtin_name(name);

	push(value);
}

void Execution::op_load_attr(unsigned int namei)
{
	auto object = pop();

	// TODO: support all object types
	auto dict = object.require<ModuleObject>().dict();

	auto name = code().names().get_item(namei);
	push(dict.get_item(name));
}

void Execution::op_compare_op(unsigned int opname)
{
	auto right = pop();
	auto left = pop();

	Object callable;
	bool not_filter = false;

	switch (opname) {
	case CompareLT:        callable = left.protocol()->lt; break;
	case CompareLE:        callable = left.protocol()->le; break;
	case CompareEQ:        callable = left.protocol()->eq; break;
	case CompareNE:        callable = left.protocol()->ne; break;
	case CompareGT:        callable = left.protocol()->gt; break;
	case CompareGE:        callable = left.protocol()->ge; break;
	case CompareIn:        callable = left.protocol()->contains; break;
	case CompareNotIn:     callable = left.protocol()->contains; not_filter = true; break;
	case CompareIs:        push(BoolObject::FromBool(left == right)); return;
	case CompareIsNot:     push(BoolObject::FromBool(left != right)); return;
	case CompareException: throw RuntimeError("CompareException not implemented");

	default:
		throw RuntimeError("Unknown comparison");
	}

	initiate_call(callable, TupleObject::New(left, right), not_filter);
}

void Execution::op_import_name(unsigned int namei)
{
	auto from = pop();
	auto level = pop().require<LongObject>();
	auto name = code().names().get_item(namei);
	auto module = Context::Active().import_builtin_module(name);

	if (!from.check<NoneObject>()) {
		auto fromlist = from.require<TupleObject>();
		assert(fromlist.size() == 1);
		push(module.cast<ModuleObject>().dict().get_item(fromlist.get_item(0)));
	}

	push(module);
}

void Execution::op_import_from(unsigned int namei)
{
	auto module = pop().require<ModuleObject>();
	auto name = code().names().get_item(namei);
	push(module.dict().get_item(name));
}

void Execution::op_load_fast(unsigned int var_num)
{
	push(dict().get_item(code().varnames().get_item(var_num)));
}

void Execution::op_store_fast(unsigned int var_num)
{
	dict().set_item(code().varnames().get_item(var_num), pop());
}

void Execution::op_call_function(uint16_t portable_argc)
{
	unsigned int argc = PortByteorder(portable_argc) & 0xff;
	unsigned int kwargc = PortByteorder(portable_argc) >> 8;

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

void Execution::op_make_function(uint16_t portable_argc)
{
	if (portable_argc)
		throw RuntimeError("default function arguments not supported");

	auto code = pop().require<CodeObject>();
	push(FunctionObject::New(code));
}

} // namespace
