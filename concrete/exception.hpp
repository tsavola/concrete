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

class Exception: public std::exception {
public:
	explicit Exception(const Object &args) throw ();
	Exception(const Exception &other) throw ();
	virtual ~Exception() throw ();

	Exception &operator=(const Exception &other) throw ();

	virtual const char *what() const throw ();

protected:
	Exception(const StringObject &repr, int) throw ();

private:
	StringObject m_repr;
};

class TypeError: public Exception {
public:
	explicit TypeError(const TypeObject &type) throw ();
};

class KeyError: public Exception {
public:
	explicit KeyError(const Object &key) throw ();
};

class RuntimeError: public Exception {
public:
	explicit RuntimeError(const char *message) throw ();
};

} // namespace

#endif
