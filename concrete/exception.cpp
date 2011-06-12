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
Exception::Exception(const Object &args) throw ():
	m_repr(args.repr())
{
	Backtrace();
}

Exception::Exception(const StringObject &repr, int) throw ():
	m_repr(repr)
{
	Backtrace();
}

Exception::Exception(const Exception &other) throw ():
	m_repr(other.m_repr)
{
}

Exception::~Exception() throw ()
{
}

Exception &Exception::operator=(const Exception &other) throw ()
{
	m_repr = other.m_repr;
	return *this;
}

const char *Exception::what() const throw ()
{
	return m_repr.data();
}

TypeError::TypeError(const TypeObject &type) throw ():
	Exception(type.name(), 0)
{
}

KeyError::KeyError(const Object &key) throw ():
	Exception(key)
{
}

RuntimeError::RuntimeError(const char *message) throw ():
	Exception(StringObject::New(message), 0)
{
}

} // namespace
