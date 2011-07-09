/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_EXECUTION_HPP
#define CONCRETE_EXECUTION_HPP

#include <cstddef>
#include <cstdint>

#include <concrete/pointer.hpp>
#include <concrete/continuation.hpp>
#include <concrete/list.hpp>
#include <concrete/objects/code.hpp>
#include <concrete/objects/dict.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/objects/tuple.hpp>
#include <concrete/portable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

class ExecutionFrame: public Pointer {
	friend class Pointer;
	friend class Execution;

private:
	struct Data: Noncopyable {
		Data(const ExecutionFrame &parent, const CodeObject &code, const DictObject &dict);
		~Data() throw ();

		unsigned int stack_size() const throw ();

		const Portable<ExecutionFrame> parent;
		const Portable<CodeObject>     code;
		const Portable<DictObject>     dict;
		Portable<uint32_t>             bytecode_position;
		Portable<Object>               call_callable;
		Portable<Continuation>         call_continuation;
		Portable<uint32_t>             stack_pointer;
		Portable<Object>               stack_objects[0]; // must be last
	} CONCRETE_PACKED;

public:
	ExecutionFrame() throw () {}
	ExecutionFrame(const ExecutionFrame &other) throw (): Pointer(other) {}

	Object result();
	void destroy() throw ();

private:
	static ExecutionFrame New(const ExecutionFrame &parent,
	                          const CodeObject &code,
	                          const DictObject &dict);

	explicit ExecutionFrame(unsigned int address) throw ():
		Pointer(address) {}

	ExecutionFrame parent() const;

	CodeObject code() const;
	DictObject dict() const;

	template <typename T> T load_bytecode();

	Object call_callable() const;
	Continuation call_continuation() const;
	void set_call(const Object &callable, Continuation cont);

	void push(const Object &object);
	Object pop();

	Data *data() const;
};

class Execution: public ListNode<Execution> {
	friend class Pointer;

public:
	static Execution New(const CodeObject &code);

	Execution() throw () {}

	Execution(const Execution &other) throw ():
		ListNode<Execution>(other) {}

	bool execute();
	ExecutionFrame new_frame(const CodeObject &code, const DictObject &dict);
	void destroy() throw ();

private:
	struct Data: ListNode<Execution>::Data {
		Data(const ExecutionFrame &frame) throw ();
		~Data() throw ();

		Portable<ExecutionFrame> initial;
		Portable<ExecutionFrame> current;
	} CONCRETE_PACKED;

	explicit Execution(unsigned int address) throw ():
		ListNode<Execution>(address) {}

	Data *data() const;
	ExecutionFrame frame() const;
	CodeObject code() const;
	DictObject dict() const;

	template <typename T> T load();

	void initiate_call(const Object &callable);
	void initiate_call(const Object &callable, const TupleObject &args);
	void initiate_call(const Object &callable, const TupleObject &args, const DictObject &kwargs);
	bool resume_call();

	void push(const Object &object);
	Object pop();

	void execute_op();

	void op_pop_top();
	void op_binary_add();
	void op_return_value();
	void op_store_name(unsigned int namei);
	void op_load_const(unsigned int consti);
	void op_load_name(unsigned int namei);
	void op_load_attr(unsigned int namei);
	void op_import_name(unsigned int namei);
	void op_import_from(unsigned int namei);
	void op_load_fast(unsigned int var_num);
	void op_call_function(uint16_t argc);
	void op_make_function(uint16_t argc);
};

} // namespace

#endif
