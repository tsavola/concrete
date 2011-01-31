/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "execute.hpp"

#include <cstdint>
#include <stdexcept>

#include <concrete/block.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/function.hpp>
#include <concrete/objects/internal.hpp>
#include <concrete/objects/long.hpp>
#include <concrete/objects/module.hpp>
#include <concrete/objects/none.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/string.hpp>
#include <concrete/objects/type.hpp>
#include <concrete/util/loader.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>
#include <concrete/util/portable.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

struct Op {
	enum Code {
		PopTop              = 1,
		Nop                 = 9,
		BinaryAdd           = 23,
		ReturnValue         = 83,
		StoreName           = 90,
		LoadConst           = 100,
		LoadName            = 101,
		LoadAttr            = 106,
		ImportName          = 108,
		ImportFrom          = 109,
		LoadFast            = 124,
		CallFunction        = 131,
		MakeFunction        = 132,
	};
};

class ExecutionState: noncopyable {
	struct FrameBlock: Block {
		const portable<BlockId> parent_id;
		const PortableCodeObject code;
		portable<uint32_t> code_position;
		const PortableDictObject dict;
		portable<uint32_t> stack_pointer;
		PortableObject stack_objects[0];

		FrameBlock(BlockId parent_id, const CodeObject &code, const DictObject &dict):
			parent_id(parent_id),
			code(code),
			code_position(0),
			dict(dict),
			stack_pointer(0)
		{
		}

	} CONCRETE_PACKED;

public:
	ExecutionState(): m_current_frame_id(NoBlockId)
	{
	}

	~ExecutionState()
	{
		while (m_current_frame_id != NoBlockId)
			delete_frame();
	}

	void new_frame(const CodeObject &code, const DictObject &dict)
	{
		concrete_trace(("new frame: code=%d") % code.id());

		auto id = Context::Alloc(
			sizeof (FrameBlock) + sizeof (PortableObject) * code.stacksize());
		new (Context::Pointer(id)) FrameBlock(m_current_frame_id, code, dict);

		m_current_frame_id = id;
	}

	Object delete_frame()
	{
		Object value;

		if (m_current_frame_id != NoBlockId && current_frame()->stack_pointer > 0)
			value = pop();

		concrete_trace(("delete frame: return object=%d type=%d")
		               % value.id() % value.type().id());

		auto id = m_current_frame_id;
		m_current_frame_id = frame(id)->parent_id;

		unsigned int top = frame(id)->stack_pointer;
		for (unsigned int i = top; i-- > 0; )
			frame(id)->stack_objects[i].~PortableObject();

		Context::Delete<FrameBlock>(id);

		return value;
	}

	CodeObject code() const
	{
		return current_frame()->code;
	}

	bool restore(Loader &loader)
	{
		// TODO: properly
		if (m_current_frame_id == NoBlockId)
			return false;

		auto frame = current_frame();
		auto bytecode = frame->code.code();

		loader.reset(bytecode.data(), bytecode.size(), frame->code_position);
		return !loader.empty();
	}

	bool save(Loader &loader)
	{
		// TODO: properly
		if (m_current_frame_id == NoBlockId)
			return false;

		auto frame = current_frame();

		frame->code_position = loader.position();
		return !loader.empty();
	}

	void push(const Object &object)
	{
		// TODO: properly
		if (m_current_frame_id == NoBlockId)
			return;

		auto frame = current_frame();
		unsigned int i = frame->stack_pointer;

		if (i >= frame->code.stacksize())
			throw std::runtime_error("stack overflow");

		new (&frame->stack_objects[i]) PortableObject(object);
		frame->stack_pointer = ++i;
	}

	Object peek(unsigned int offset = 0)
	{
		auto frame = current_frame();
		unsigned int i = frame->stack_pointer;

		if (i <= offset)
			throw std::runtime_error("stack underflow");

		return Object(frame->stack_objects[i - offset - 1]);
	}

	Object pop()
	{
		// TODO: properly
		if (m_current_frame_id == NoBlockId)
			return Object();

		auto frame = current_frame();
		unsigned int i = frame->stack_pointer;

		if (i == 0)
			throw std::runtime_error("stack underflow");

		frame->stack_pointer = --i;
		Object object(frame->stack_objects[i]);

		frame->stack_objects[i].~PortableObject();

		return object;
	}

	void pop(unsigned int count)
	{
		auto frame = current_frame();
		unsigned int i = frame->stack_pointer;

		if (i < count)
			throw std::runtime_error("stack underflow");

		frame->stack_pointer = i - count;

		for (unsigned int n = 1; n <= count; n++)
			frame->stack_objects[i - n].~PortableObject();
	}

	DictObject dict()
	{
		return current_frame()->dict;
	}

private:
	static FrameBlock *frame(BlockId id)
	{
		return static_cast<FrameBlock *> (Context::Pointer(id));
	}

	FrameBlock *current_frame() const
	{
		return frame(m_current_frame_id);
	}

	BlockId m_current_frame_id;
};

class CodeExecutor: noncopyable {
public:
	CodeExecutor(const CodeObject &code)
	{
		m_state.new_frame(code, DictObject::New());
	}

	bool execute()
	{
		if (!m_state.restore(m_loader))
			return false;

		execute_op();

		if (!m_state.save(m_loader))
			return false;

		return true;
	}

private:
	void execute_op()
	{
		unsigned int pos = m_loader.position();
		unsigned int op = load<uint8_t>();

		concrete_trace(("position=%d opcode=%u") % pos % op);

		switch (Op::Code(op)) {
		case Op::PopTop:              op_pop_top(); break;
		case Op::Nop:                 break;
		case Op::BinaryAdd:           op_binary_add(); break;
		case Op::ReturnValue:         op_return_value(); break;
		case Op::StoreName:           op_store_name(); break;
		case Op::LoadConst:           op_load_const(); break;
		case Op::LoadName:            op_load_name(); break;
		case Op::LoadAttr:            op_load_attr(); break;
		case Op::ImportName:          op_import_name(); break;
		case Op::ImportFrom:          op_import_from(); break;
		case Op::LoadFast:            op_load_fast(); break;
		case Op::CallFunction:        op_call_function(); break;
		case Op::MakeFunction:        op_make_function(); break;

		default:
			concrete_trace(("unsupported opcode: %u") % op);
			throw std::runtime_error("unsupported opcode");
		}
	}

	void op_pop_top()
	{
		m_state.pop();
	}

	void op_binary_add()
	{
		auto b = m_state.pop();
		auto a = m_state.pop();

		auto args = TupleObject::New(2);
		args.init_item(0, a);
		args.init_item(1, b);

		// TODO: support user functions
		auto func = a.type().protocol().add.require<InternalObject>("internal object expected");

		m_state.push(func.call(args, DictObject::New(0)));
	}

	void op_return_value()
	{
		auto value = m_state.delete_frame();
		m_state.push(value);
		m_state.restore(m_loader);

		concrete_trace(("return_value: %s") % value.repr().data());
	}

	void op_store_name()
	{
		auto name = m_state.code().names().get_item(load<uint16_t>());
		auto value = m_state.pop();
		m_state.dict().set_item(name, value);
	}

	void op_load_const()
	{
		m_state.push(m_state.code().consts().get_item(load<uint16_t>()));
	}

	void op_load_name()
	{
		auto name = m_state.code().names().get_item(load<uint16_t>());
		m_state.push(m_state.dict().get_item(name));
	}

	void op_load_attr()
	{
		auto object = m_state.pop();

		// TODO
		auto module = object.require<ModuleObject>("only module objects supported for now");
		auto dict = module.dict();

		auto name = m_state.code().names().get_item(load<uint16_t>());
		m_state.push(dict.get_item(name));
	}

	void op_import_name()
	{
		auto from = m_state.pop();
		auto level = m_state.pop().require<LongObject>("long object expected");
		auto name = m_state.code().names().get_item(load<uint16_t>());
		auto module = Context::ImportBuiltin(name);

		if (!from.check<NoneObject>()) {
			auto fromlist = from.require<TupleObject>("tuple object expected");
			assert(fromlist.size() == 1);
			m_state.push(module.cast<ModuleObject>().dict().get_item(fromlist.get_item(0)));
		}

		m_state.push(module);
	}

	void op_import_from()
	{
		auto module = m_state.pop().require<ModuleObject>("module object expected");
		auto name = m_state.code().names().get_item(load<uint16_t>());
		m_state.push(module.dict().get_item(name));
	}

	void op_load_fast()
	{
		auto varname = m_state.code().varnames().get_item(load<uint16_t>());
		m_state.push(m_state.dict().get_item(varname));
	}

	void op_call_function()
	{
		unsigned int argc = load<uint8_t>();
		unsigned int kwargc = load<uint8_t>();

		auto object = m_state.peek(argc + kwargc * 2);

		if (object.check<FunctionObject>()) {
			auto function = object.cast<FunctionObject>();

			auto dict = DictObject::New(argc + kwargc);

			for (unsigned int i = 0; i < kwargc; i++) {
				auto value = m_state.pop();
				auto key = m_state.pop();

				dict.set_item(key, value);
			}

			for (unsigned int i = argc; i-- > 0; ) {
				auto value = m_state.pop();
				auto key = function.code().varnames().get_item(i);

				dict.set_item(key, value);
			}

			m_state.pop(1); // function

			m_state.save(m_loader);
			m_state.new_frame(function.code(), dict);
			m_loader.reset(function.code().code().data(), function.code().code().size());
			return;
		}

		if (object.check<InternalObject>()) {
			auto internal = object.cast<InternalObject>();

			auto kwargs = DictObject::New(kwargc);
			for (unsigned int i = 0; i < kwargc; i++) {
				auto value = m_state.pop();
				auto key = m_state.pop();

				kwargs.set_item(key, value);
			}

			auto args = TupleObject::New(argc);
			for (unsigned int i = argc; i-- > 0; )
				args.init_item(i, m_state.pop());

			m_state.pop(1); // internal

			m_state.push(internal.call(args, kwargs));
			return;
		}

		throw std::runtime_error("function or internal object expected");
	}

	void op_make_function()
	{
		if (load<uint16_t>() > 0)
			throw std::runtime_error("default function arguments not supported");

		auto code = m_state.pop().require<CodeObject>("code object expected");
		m_state.push(FunctionObject::New(code));
	}

	template <typename T>
	T load()
	{
		return m_loader.load<T>();
	}

	ExecutionState m_state;
	Loader m_loader;
};

Executor::Executor(const CodeObject &code): m_impl(new CodeExecutor(code))
{
}

Executor::~Executor()
{
	delete m_impl;
}

bool Executor::execute()
{
	return m_impl->execute();
}

} // namespace
