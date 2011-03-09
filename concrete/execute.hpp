/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_EXECUTE_HPP
#define CONCRETE_EXECUTE_HPP

#include <concrete/objects/code.hpp>
#include <concrete/util/activatable.hpp>
#include <concrete/util/noncopyable.hpp>

namespace concrete {

class CodeExecutor;

class Executor: activatable<Executor>, noncopyable {
	friend class activatable<Executor>;
	friend class active_scope<Executor>;
public:
	static Executor &Active()
	{
		return activatable<Executor>::Active();
	}

	explicit Executor(const CodeObject &code);
	~Executor();

	bool execute();

private:
	CodeExecutor *m_impl;
};

} // namespace

#endif
