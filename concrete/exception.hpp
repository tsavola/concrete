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
#include <concrete/objects/type-decl.hpp>

namespace concrete {

class Exception: public std::exception {
public:
	explicit Exception(const Object &args) throw (): m_args(args)
	{
	}

	Exception(const Exception &other) throw (): m_args(other.m_args)
	{
	}

	virtual ~Exception() throw ()
	{
	}

	Exception &operator=(const Exception &other) throw ()
	{
		m_args = other.m_args;
		return *this;
	}

	virtual const char *what() const throw ();

protected:
	Object m_args;
};

class TypeError: public Exception {
public:
	explicit TypeError(const TypeObject &type) throw (): Exception(type)
	{
	}

	virtual const char *what() const throw ();
};

class KeyError: public Exception {
public:
	explicit KeyError(const Object &key) throw (): Exception(key)
	{
	}
};

} // namespace

#endif
