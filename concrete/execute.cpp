/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "execute.hpp"

#include <cassert>
#include <cstdint>

#include <concrete/block.hpp>
#include <concrete/exception.hpp>
#include <concrete/objects/callable.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/function.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/none.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/opcodes.hpp>
#include <concrete/util/loader.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

struct ExecutionFrame: Block {
	const Portable<CodeObject> code;
	Portable<uint32_t> code_position;

	const Portable<DictObject> dict;

	const Portable<BlockId> parent_frame_id;

	Portable<BlockId> call_continuation;
	Portable<Object> call_callable;

	Portable<uint32_t> stack_pointer;
	Portable<Object> stack_objects[0];      // must be last

	ExecutionFrame(const CodeObject &code,
	               const DictObject &dict,
	               BlockId parent_frame_id = 0):
		code(code),
		code_position(0),
		dict(dict),
		parent_frame_id(parent_frame_id),
		stack_pointer(0)
	{
	}

	~ExecutionFrame() throw ()
	{
		if (call_continuation) {
			ConcreteTrace(("call cleanup enter: callable=%d") % call_callable.id());

			call_callable->cast<CallableObject>().cleanup_call(call_continuation);

			ConcreteTrace(("call cleanup leave: callable=%d") % call_callable.id());
		}

		for (unsigned int i = stack_pointer; i-- > 0; )
			stack_objects[i].~Portable<Object>();
	}

	void push_stack(const Object &object)
	{
		unsigned int i = stack_pointer;
		if (i >= code->stacksize())
			throw RuntimeError("stack overflow");

		new (&stack_objects[i]) Portable<Object>(object);
		stack_pointer = ++i;
	}

	Object pop_stack()
	{
		unsigned int i = stack_pointer;
		if (i == 0)
			throw RuntimeError("stack underflow");

		stack_pointer = --i;
		Object object(stack_objects[i]);
		stack_objects[i].~Portable<Object>();

		return object;
	}

	static size_t BlockSize(const CodeObject &code)
	{
		return sizeof (ExecutionFrame) + sizeof (Portable<Object>) * code.stacksize();
	}
} CONCRETE_PACKED;

class ExecutionState: Noncopyable {
public:
	explicit ExecutionState(const CodeObject &code)
	{
		auto dict = DictObject::New();

		auto frame_id = Context::NewCustomSizeBlock<ExecutionFrame>(
			ExecutionFrame::BlockSize(code), code, dict);

		m_initial_frame_id = frame_id;
		m_current_frame_id = frame_id;

		ConcreteTrace(("frame new initial %d") % m_initial_frame_id);
	}

	explicit ExecutionState(const Executor::Snapshot &snapshot) throw ():
		m_initial_frame_id(snapshot.initial_frame_id),
		m_current_frame_id(snapshot.current_frame_id)
	{
	}

	~ExecutionState() throw ()
	{
		ConcreteTrace(("frame destroy initial %d") % m_initial_frame_id);

		Context::DeleteBlock<ExecutionFrame>(m_initial_frame_id);
	}

	Executor::Snapshot snapshot() const throw ()
	{
		return Executor::Snapshot(m_initial_frame_id, m_current_frame_id);
	}

	/*
	 * Frame management
	 */

	BlockId new_frame(const CodeObject &code, const DictObject &dict)
	{
		auto old_frame_id = m_current_frame_id;

		auto new_frame_id = Context::NewCustomSizeBlock<ExecutionFrame>(
			ExecutionFrame::BlockSize(code), code, dict, old_frame_id);

		m_current_frame_id = new_frame_id;

		ConcreteTrace(("frame new %d: code=%d") % new_frame_id % code.id());

		return new_frame_id;
	}

	void exit_frame()
	{
		ConcreteTrace(("frame exit %d") % m_current_frame_id);

		m_current_frame_id = current_frame()->parent_frame_id;
	}

	Object frame_result(BlockId frame_id)
	{
		ConcreteTrace(("frame result %d") % frame_id);

		return Frame(frame_id)->pop_stack();
	}

	void destroy_frame(BlockId frame_id) throw ()
	{
		ConcreteTrace(("frame destroy %d") % frame_id);

		Context::DeleteBlock<ExecutionFrame>(frame_id);
	}

	bool have_frame() throw ()
	{
		return m_current_frame_id;
	}

	/*
	 * Properties
	 */

	CodeObject frame_code() const
	{
		return current_frame()->code;
	}

	Portable<uint32_t> &frame_position()
	{
		return current_frame()->code_position;
	}

	DictObject frame_dict() const
	{
		return current_frame()->dict;
	}

	/*
	 * Stack operations
	 */

	void push_stack(const Object &object)
	{
		current_frame()->push_stack(object);
	}

	Object pop_stack()
	{
		return current_frame()->pop_stack();
	}

	/*
	 * Continuation operations
	 */

	void init_call(const Object &callable, const TupleObject &args, const DictObject &kwargs)
	{
		BlockId frame_id = m_current_frame_id;
		BlockId continuation = 0;

		ConcreteTrace(("call init enter: callable=%d frame=%d")
		              % callable.id() % frame_id);

		auto result = callable.require<CallableObject>().init_call(continuation, args, kwargs);

		ConcreteTrace(("call init leave: callable=%d frame=%d done=%s return=%d")
		              % callable.id() % frame_id
		              % (continuation ? "false" : "true") % result.id());

		auto frame = Frame(frame_id);

		if (continuation) {
			frame->call_continuation = continuation;
			frame->call_callable = callable;
		} else {
			frame->push_stack(result);
		}
	}

	bool resume_call()
	{
		BlockId frame_id = m_current_frame_id;
		ExecutionFrame *frame;
		BlockId continuation;

		frame = Frame(frame_id);
		continuation = frame->call_continuation;

		if (continuation == 0)
			return false;

		Object callable = frame->call_callable;

		ConcreteTrace(("call resume enter: callable=%d frame=%d")
		              % callable.id() % frame_id);

		auto result = callable.cast<CallableObject>().resume_call(continuation);

		ConcreteTrace(("call resume leave: callable=%d frame=%d done=%d return=%d")
		              % callable.id() % frame_id
		              % (continuation ? "false" : "true") % result.id());

		frame = Frame(frame_id);
		frame->call_continuation = continuation;

		if (continuation == 0) {
			frame->call_callable = Object();
			frame->push_stack(result);
		}

		return true;
	}

private:
	static ExecutionFrame *Frame(BlockId id)
	{
		return Context::BlockPointer<ExecutionFrame>(id);
	}

	ExecutionFrame *current_frame() const
	{
		return Frame(m_current_frame_id);
	}

	BlockId m_initial_frame_id;
	BlockId m_current_frame_id;
};

class BytecodeLoader: public Loader<BytecodeLoader> {
public:
	BytecodeLoader(ExecutionState &execution) throw ():
		Loader<BytecodeLoader>(*this),
		m_execution(execution)
	{
	}

	const uint8_t *data() const
	{
		return m_execution.frame_code().code().data();
	}

	size_t size() const
	{
		return m_execution.frame_code().code().size();
	}

	size_t position() const
	{
		return m_execution.frame_position();
	}

	void advance(size_t offset)
	{
		auto &position = m_execution.frame_position();
		position = position + offset;
	}

private:
	ExecutionState &m_execution;
};

class Executor::Impl: public ExecutionState {
public:
	explicit Impl(const CodeObject &code):
		ExecutionState(code),
		m_loader(*this)
	{
	}

	explicit Impl(const Snapshot &snapshot) throw ():
		ExecutionState(snapshot),
		m_loader(*this)
	{
	}

	bool execute()
	{
		if (have_frame()) {
			if (resume_call() || execute_opcode())
				return have_frame();
		}

		return false;
	}

private:
	bool execute_opcode()
	{
		using namespace opcodes;

		if (m_loader.empty())
			return false;

		auto pos = m_loader.position();
		unsigned int op = load_bytecode<uint8_t>();

		ConcreteTrace(("execute %4u: opcode=%u") % pos % op);

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
			ConcreteTrace(("unsupported opcode: %u") % op);
			throw RuntimeError("unsupported opcode");
		}

		return true;
	}

	void op_pop_top()
	{
		pop_stack();
	}

	void op_binary_add()
	{
		auto b = pop_stack();
		auto a = pop_stack();
		init_call(a.protocol().add, TupleObject::New(a, b), DictObject::EmptySingleton());
	}

	void op_return_value()
	{
		exit_frame();
	}

	void op_store_name()
	{
		auto name = frame_code().names().get_item(load_bytecode<uint16_t>());
		auto value = pop_stack();
		frame_dict().set_item(name, value);
	}

	void op_load_const()
	{
		push_stack(frame_code().consts().get_item(load_bytecode<uint16_t>()));
	}

	void op_load_name()
	{
		auto name = frame_code().names().get_item(load_bytecode<uint16_t>());
		Object value;

		if (!frame_dict().get_item(name, value))
			value = Context::LoadBuiltinName(name);

		push_stack(value);
	}

	void op_load_attr()
	{
		auto object = pop_stack();

		// TODO: support all object types
		auto dict = object.require<ModuleObject>().dict();

		auto name = frame_code().names().get_item(load_bytecode<uint16_t>());
		push_stack(dict.get_item(name));
	}

	void op_import_name()
	{
		auto from = pop_stack();
		auto level = pop_stack().require<LongObject>();
		auto name = frame_code().names().get_item(load_bytecode<uint16_t>());
		auto module = Context::ImportBuiltinModule(name);

		if (!from.check<NoneObject>()) {
			auto fromlist = from.require<TupleObject>();
			assert(fromlist.size() == 1);
			push_stack(module.cast<ModuleObject>().dict().get_item(fromlist.get_item(0)));
		}

		push_stack(module);
	}

	void op_import_from()
	{
		auto module = pop_stack().require<ModuleObject>();
		auto name = frame_code().names().get_item(load_bytecode<uint16_t>());
		push_stack(module.dict().get_item(name));
	}

	void op_load_fast()
	{
		auto varname = frame_code().varnames().get_item(load_bytecode<uint16_t>());
		push_stack(frame_dict().get_item(varname));
	}

	void op_call_function()
	{
		unsigned int argc = load_bytecode<uint8_t>();
		unsigned int kwargc = load_bytecode<uint8_t>();

		auto args = TupleObject::NewWithSize(argc);
		auto kwargs = DictObject::NewWithCapacity(kwargc);

		for (unsigned int i = 0; i < kwargc; i++) {
			auto value = pop_stack();
			auto key = pop_stack();

			kwargs.set_item(key, value);
		}

		for (unsigned int i = argc; i-- > 0; )
			args.init_item(i, pop_stack());

		init_call(pop_stack(), args, kwargs);
	}

	void op_make_function()
	{
		if (load_bytecode<uint16_t>() > 0)
			throw RuntimeError("default function arguments not supported");

		auto code = pop_stack().require<CodeObject>();
		push_stack(FunctionObject::New(code));
	}

	template <typename T>
	T load_bytecode()
	{
		return m_loader.load<T>();
	}

	BytecodeLoader m_loader;
};

Executor::Snapshot::Snapshot() throw ()
{
}

Executor::Snapshot::Snapshot(BlockId initial_frame_id, BlockId current_frame_id) throw ():
	initial_frame_id(initial_frame_id),
	current_frame_id(current_frame_id)
{
}

size_t Executor::Snapshot::size() const throw ()
{
	return sizeof (*this);
}

const void *Executor::Snapshot::ptr() const throw ()
{
	return reinterpret_cast<const void *> (this);
}

void *Executor::Snapshot::ptr() throw ()
{
	return reinterpret_cast<void *> (this);
}

Executor &Executor::Active() throw ()
{
	return Activatable<Executor>::Active();
}

Executor::Executor(const CodeObject &code):
	m_impl(new Impl(code))
{
}

Executor::Executor(const Snapshot &snapshot):
	m_impl(new Impl(snapshot))
{
}

Executor::~Executor() throw ()
{
	ActiveScope<Executor> scope(*this);
	delete m_impl;
}

Executor::Snapshot Executor::snapshot() const throw ()
{
	return m_impl->snapshot();
}

bool Executor::execute()
{
	ActiveScope<Executor> scope(*this);
	return m_impl->execute();
}

BlockId Executor::new_frame(const CodeObject &code, const DictObject &dict)
{
	return m_impl->new_frame(code, dict);
}

Object Executor::frame_result(BlockId id)
{
	return m_impl->frame_result(id);
}

void Executor::destroy_frame(BlockId id) throw ()
{
	m_impl->destroy_frame(id);
}

} // namespace
