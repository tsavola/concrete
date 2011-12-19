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

#include <concrete/objects/object-partial.hpp>
#include <concrete/objects/string.hpp>

namespace concrete {

class TypeObject;

class CodeException: public std::exception {
public:
	explicit CodeException(const Object &args) throw ();
	CodeException(const CodeException &other) throw (): m_repr(other.m_repr) {}

	void operator=(const CodeException &other) throw () { m_repr = other.m_repr; }

	virtual const char *what() const throw () { return m_repr.c_str(); }

protected:
	CodeException(const StringObject &repr, int) throw ();

private:
	StringObject m_repr;
};

class TypeError: public CodeException {
public:
	explicit TypeError(const TypeObject &type) throw ();
};

class KeyError: public CodeException {
public:
	explicit KeyError(const Object &key) throw (): CodeException(key) {}
};

class RuntimeError: public CodeException {
public:
	explicit RuntimeError(const char *message) throw ();
};

} // namespace

#endif
