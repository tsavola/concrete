/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_EXCEPTION_HPP
#define CONCRETE_EXCEPTION_HPP

#include <exception>

#include <concrete/objects/object-decl.hpp>
#include <concrete/objects/string-decl.hpp>
#include <concrete/objects/type-decl.hpp>

namespace concrete {

class Exception: public std::exception {
public:
	// TODO
	explicit Exception(const Object &args) throw (): m_repr(args.repr())
	{
	}

	Exception(const Exception &other) throw (): m_repr(other.m_repr)
	{
	}

	virtual ~Exception() throw ()
	{
	}

	Exception &operator=(const Exception &other) throw ()
	{
		m_repr = other.m_repr;
		return *this;
	}

	virtual const char *what() const throw ()
	{
		return m_repr.data();
	}

protected:
	Exception(const StringObject &repr, int) throw (): m_repr(repr)
	{
	}

private:
	StringObject m_repr;
};

class TypeError: public Exception {
public:
	explicit TypeError(const TypeObject &type) throw (): Exception(type.name(), 0)
	{
	}
};

class KeyError: public Exception {
public:
	explicit KeyError(const Object &key) throw (): Exception(key)
	{
	}
};

class RuntimeError: public Exception {
public:
	explicit RuntimeError(const char *message) throw (): Exception(StringObject::New(message), 0)
	{
	}
};

} // namespace

#endif
