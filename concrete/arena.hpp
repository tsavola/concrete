/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_ARENA_HPP
#define CONCRETE_ARENA_HPP

#include "arena-access.hpp"

#include <cstddef>
#include <cstdint>
#include <exception>

#include <concrete/portable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

class Arena: Noncopyable {
private:
	struct Header {
		explicit Header(size_t size) throw (): size(size) {}

		const Portable<uint32_t> size;
	} CONCRETE_PACKED;

public:
	enum { InitialAllocationAddress = sizeof (Header) };

	struct Snapshot {
		const void *base;
		size_t      size;
	};

	struct Allocation {
		unsigned int  address;
		void         *data;
	};

	struct Accession {
		unsigned int  version;
		void         *data;
	};

	static Arena &Active() throw ();
	static size_t AllocationSize(const void *data) throw ();
	static size_t AlignedSize(size_t size) throw ();

	Arena() throw ();
	Arena(void *base, size_t size);
	~Arena() throw ();

	Snapshot snapshot() const throw ();

	Allocation allocate(size_t size);
	void free(unsigned int address, void *data) throw ();

	unsigned int version() const throw () { return m_version; }

	Accession access(unsigned int address, size_t minimum_size);
	Accession nonthrowing_access(unsigned int address, size_t minimum_size) throw ();

	Accession defer_access_error(unsigned int address) throw ();
	void check_access_error();

private:
	static const Header *AllocationHeader(const void *data) throw ();
	static Header *AllocationHeader(void *data) throw ();

	void update_version() throw ();

	void         *m_base;
	size_t        m_size;
	unsigned int  m_version;
	unsigned int  m_access_error_address;
};

class AllocationError: std::exception {
public:
	explicit AllocationError(size_t size) throw ();

	virtual const char *what() const throw ();
	size_t size() const throw () { return m_size; }

private:
	const size_t m_size;
};

} // namespace

#endif
