/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_PORTABLE_HPP
#define CONCRETE_PORTABLE_HPP

#include <cstdint>

#include <concrete/util/byteorder.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

template <typename ValueType>
struct PortableRawAccess {
	typedef typename ValueType::RawAccess::RawType RawType;

	static RawType InitRef() throw ();
	static void Ref(RawType raw) throw ();
	static void Unref(RawType raw) throw ();
	static RawType Extract(const ValueType &value) throw ();
	static RawType ExtractRef(const ValueType &value) throw ();
	static ValueType Materialize(RawType raw) throw ();
};

template <typename ValueType, typename PrimitiveType = ValueType>
struct PortablePrimitiveAccess {
	typedef PrimitiveType RawType;

	static PrimitiveType InitRef() throw ();
	static void Ref(PrimitiveType raw) throw () {};
	static void Unref(PrimitiveType raw) throw () {};
	static PrimitiveType Extract(ValueType value) throw ();
	static PrimitiveType ExtractRef(ValueType value) throw ();
	static ValueType Materialize(PrimitiveType raw) throw ();
};

template <> struct PortableRawAccess<bool>: PortablePrimitiveAccess<bool, uint8_t> {};
template <> struct PortableRawAccess<int8_t>: PortablePrimitiveAccess<int8_t> {};
template <> struct PortableRawAccess<int16_t>: PortablePrimitiveAccess<int16_t> {};
template <> struct PortableRawAccess<int32_t>: PortablePrimitiveAccess<int32_t> {};
template <> struct PortableRawAccess<int64_t>: PortablePrimitiveAccess<int64_t> {};
template <> struct PortableRawAccess<uint8_t>: PortablePrimitiveAccess<uint8_t> {};
template <> struct PortableRawAccess<uint16_t>: PortablePrimitiveAccess<uint16_t> {};
template <> struct PortableRawAccess<uint32_t>: PortablePrimitiveAccess<uint32_t> {};
template <> struct PortableRawAccess<uint64_t>: PortablePrimitiveAccess<uint64_t> {};

template <typename ValueType>
class Portable {
	typedef PortableRawAccess<ValueType> RawAccess;
	typedef typename PortableRawAccess<ValueType>::RawType RawType;
	typedef PortableByteorder<RawType, sizeof (RawType)> Byteorder;

	class Pointer {
		friend class Portable;

	public:
		Pointer(const Pointer &other) throw ();

		ValueType *operator->() throw ();

	private:
		explicit Pointer(const ValueType &value) throw ();

		void operator=(const Pointer &); // not implemented

		ValueType m_value;
	};

public:
	Portable() throw ();
	Portable(const Portable &other) throw ();
	Portable(const ValueType &value) throw ();
	~Portable() throw ();

	void operator=(const Portable &other) throw ();
	void operator=(const ValueType &value) throw ();

	bool operator==(const Portable &other) const throw ();
	bool operator!=(const Portable &other) const throw ();

	bool operator==(const ValueType &value) const throw ();
	bool operator!=(const ValueType &value) const throw ();

	operator ValueType() const throw ();
	bool operator!() const throw ();
	ValueType operator*() const throw ();
	Pointer operator->() const throw ();

private:
	void ref() throw ();
	void unref() throw ();

	RawType native_raw() const throw ();

	RawType m_raw;
} CONCRETE_PACKED;

} // namespace

#include "portable-inline.hpp"

#endif
