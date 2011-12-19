/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "exception.hpp"

#include <concrete/objects/type.hpp>
#include <concrete/util/backtrace.hpp>

namespace concrete {

// TODO
CodeException::CodeException(const Object &args) throw ():
	m_repr(args.repr())
{
	Backtrace();
}

CodeException::CodeException(const StringObject &repr, int) throw ():
	m_repr(repr)
{
	Backtrace();
}

TypeError::TypeError(const TypeObject &type) throw ():
	CodeException(type.name(), int())
{
}

RuntimeError::RuntimeError(const char *message) throw ():
	CodeException(StringObject::New(message), 0)
{
}

} // namespace
