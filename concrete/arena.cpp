/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "arena.hpp"

#include <cstdlib>
#include <cstring>

#include <concrete/context.hpp>
#include <concrete/util/backtrace.hpp>
#include <concrete/util/trace.hpp>

namespace concrete {

ArenaAccess::ArenaAccess(const ArenaAccess &other) throw ():
	m_version(other.m_version),
	m_data(other.m_data)
{
}

void ArenaAccess::operator=(const ArenaAccess &other) throw ()
{
	m_version = other.m_version;
	m_data = other.m_data;
}

void *ArenaAccess::arena_access(unsigned int address, size_t minimum_size) const
{
	Arena &arena = Arena::Active();

	if (m_version != arena.version()) {
		auto accession = arena.access(address, minimum_size);

		m_version = accession.version;
		m_data = accession.data;
	}

	return m_data;
}

void *ArenaAccess::nonthrowing_arena_access(unsigned int address, size_t minimum_size) const throw ()
{
	Arena &arena = Arena::Active();

	if (m_version != arena.version()) {
		auto accession = arena.nonthrowing_access(address, minimum_size);
		if (accession.data == NULL)
			return NULL;

		m_version = accession.version;
		m_data = accession.data;
	}

	return m_data;
}

Arena &Arena::Active() throw ()
{
	return Context::Active().arena();
}

const Arena::Header *Arena::AllocationHeader(const void *data) throw ()
{
	return reinterpret_cast<const Header *> (data) - 1;
}

Arena::Header *Arena::AllocationHeader(void *data) throw ()
{
	return reinterpret_cast<Header *> (data) - 1;
}

size_t Arena::AllocationSize(const void *data) throw ()
{
	return AllocationHeader(data)->size;
}

size_t Arena::AlignedSize(size_t size) throw ()
{
	return (size + sizeof (uint32_t) - 1) & ~size_t(sizeof (uint32_t) - 1);
}

Arena::Arena() throw ():
	m_base(NULL),
	m_size(0),
	m_version(1),
	m_access_error_address(0)
{
}

Arena::Arena(void *base, size_t size):
	m_base(base),
	m_size(size),
	m_version(1),
	m_access_error_address(0)
{
}

Arena::~Arena() throw ()
{
	std::free(m_base);
}

Arena::Snapshot Arena::snapshot() const throw ()
{
	return Snapshot {
		m_base,
		m_size,
	};
}

Arena::Allocation Arena::allocate(size_t data_size)
{
	check_access_error();

	size_t full_size = sizeof (Header) + AlignedSize(data_size);
	if (full_size < data_size)
		throw AllocationError(data_size);

	size_t old_size = m_size;
	unsigned int header_address = old_size;
	size_t new_size = old_size + full_size;

	void *new_base = std::realloc(m_base, new_size);
	if (new_base == NULL)
		throw AllocationError(data_size);

	if (new_base != m_base)
		update_version();

	m_base = new_base;
	m_size = new_size;

	char *ptr = reinterpret_cast<char *> (m_base) + header_address;
	std::memset(ptr, 0, full_size);
	new (ptr) Header(data_size);

	unsigned int data_address = header_address + sizeof (Header);
	void *data_ptr = ptr + sizeof (Header);

	return Allocation {
		data_address,
		data_ptr,
	};
}

void Arena::free(unsigned int data_address, void *data) throw ()
{
	unsigned int header_address = data_address - sizeof (Header);
	void *ptr = reinterpret_cast<void *> (reinterpret_cast<char *> (data) - sizeof (Header));

	size_t full_size = sizeof (Header) + AlignedSize(AllocationSize(data));

	if (header_address + full_size == m_size) {
		m_base = std::realloc(m_base, header_address);
		m_size = header_address;

		update_version();
	} else {
		std::memset(ptr, 0x55, full_size);
	}
}

void Arena::update_version() throw ()
{
	unsigned int version = m_version + 1;

	if (version == 0)
		version = 1;

	m_version = version;
}

Arena::Accession Arena::access(unsigned int address, size_t minimum_size)
{
	check_access_error();
	auto accession = nonthrowing_access(address, minimum_size);
	check_access_error();

	return accession;
}

Arena::Accession Arena::nonthrowing_access(unsigned int address, size_t minimum_size) throw ()
{
	if (address < sizeof (Header)) {
		Trace("address %1% access (minimum size %2%)", address, minimum_size);
		return defer_access_error(address);
	}

#ifndef NDEBUG
	if (address & (sizeof (uint32_t) - 1)) {
		Trace("address %1% alignment error", address);
		return defer_access_error(address);
	}
#endif

	if (m_size < minimum_size || address > m_size - minimum_size) {
		Trace("address %1% minimum size %2% out of range", address, minimum_size);
		return defer_access_error(address);
	}

	void *data = reinterpret_cast<char *> (m_base) + address;

	size_t data_size = AllocationSize(data);
	size_t aligned_size = AlignedSize(data_size);

	if (aligned_size < data_size)
		return defer_access_error(address);

	if (m_size < aligned_size || address > m_size - aligned_size) {
		Trace("address %1% actual size %2% out of range", address, minimum_size);
		return defer_access_error(address);
	}

	return Accession {
		m_version,
		data,
	};
}

Arena::Accession Arena::defer_access_error(unsigned int address) throw ()
{
	if (m_access_error_address == 0) {
		Backtrace();
		m_access_error_address = address;
	}

	return Accession {
		0,
		NULL,
	};
}

void Arena::check_access_error()
{
	if (m_access_error_address) {
		auto address = m_access_error_address;
		m_access_error_address = 0;

		throw IntegrityError(address);
	}
}

AllocationError::AllocationError(size_t size) throw ():
	m_size(size)
{
	Backtrace();
}

const char *AllocationError::what() const throw ()
{
	return "Out of memory";
}

IntegrityError::IntegrityError(unsigned int address) throw ():
	m_address(address)
{
	Backtrace();
}

const char *IntegrityError::what() const throw ()
{
	return "Arena integrity violation";
}

} // namespace
