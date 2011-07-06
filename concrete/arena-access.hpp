/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_ARENA_ACCESS_HPP
#define CONCRETE_ARENA_ACCESS_HPP

#include <cstddef>
#include <exception>

namespace concrete {

class ArenaAccess {
public:
	void operator=(const ArenaAccess &other) throw ();

protected:
	ArenaAccess() throw (): m_version(0) {}
	ArenaAccess(const ArenaAccess &other) throw ();

	void *arena_access(unsigned int address, size_t minimum_size) const;
	void *nonthrowing_arena_access(unsigned int address, size_t minimum_size) const throw ();

private:
	mutable unsigned int  m_version;
	mutable void         *m_data;
};

class IntegrityError: public std::exception {
public:
	explicit IntegrityError(unsigned int address) throw ();

	virtual const char *what() const throw ();
	unsigned int address() const throw () { return m_address; }

private:
	const unsigned int m_address;
};

} // namespace

#endif
