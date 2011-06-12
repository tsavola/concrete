/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_PORTABLE_HPP
#define CONCRETE_UTIL_PORTABLE_HPP

#include <cstddef>

#include <concrete/util/byteorder.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

template <typename ValueType>
struct PortableRawAccess {
	typedef typename ValueType::RawAccess::RawType RawType;

	static RawType Default() throw ()
	{
		return ValueType::RawAccess::Default();
	}

	static RawType Extract(const ValueType &value) throw ()
	{
		return ValueType::RawAccess::Extract(value);
	}

	static ValueType Materialize(RawType raw) throw ()
	{
		ValueType *type = NULL; // unnecesary initialization to suppress warning
		return ValueType::RawAccess::Materialize(raw, type);
	}

	static void Ref(RawType raw) throw ()
	{
		ValueType::RawAccess::Ref(raw);
	}

	static void Unref(RawType raw) throw ()
	{
		ValueType::RawAccess::Unref(raw);
	}
};

template <typename ValueType, typename PrimitiveType = ValueType>
struct PortablePrimitiveAccess {
	typedef PrimitiveType RawType;

	static RawType Default() throw ()
	{
		return 0;
	}

	static RawType Extract(const ValueType &value) throw ()
	{
		return RawType(value);
	}

	static ValueType Materialize(RawType raw) throw ()
	{
		return ValueType(raw);
	}

	static void Ref(RawType raw) throw ()
	{
	}

	static void Unref(RawType raw) throw ()
	{
	}
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

struct PortableRawInit {};

template <typename ValueType>
class Portable {
	typedef PortableRawAccess<ValueType> RawAccess;
	typedef typename PortableRawAccess<ValueType>::RawType RawType;
	typedef PortableByteorder<RawType, sizeof (RawType)> Byteorder;

	class Pointer {
	public:
		explicit Pointer(const ValueType &value) throw ():
			m_value(value)
		{
		}

		Pointer(const Pointer &other) throw ():
			m_value(other.m_value)
		{
		}

		void operator=(const Pointer &other) throw ()
		{
			m_value = other.m_value;
		}

		const ValueType *operator->() const throw ()
		{
			return &m_value;
		}

	private:
		ValueType m_value;
	};

public:
	Portable() throw ():
		m_raw(Byteorder::Adapt(RawAccess::Default()))
	{
		ref();
	}

	Portable(const Portable &other) throw ():
		m_raw(other.m_raw)
	{
		ref();
	}

	Portable(const ValueType &value) throw ():
		m_raw(Byteorder::Adapt(RawAccess::Extract(value)))
	{
		ref();
	}

	Portable(RawType raw, PortableRawInit) throw ():
		m_raw(Byteorder::Adapt(raw))
	{
		ref();
	}

	~Portable() throw ()
	{
		unref();
	}

	void operator=(const Portable &other) throw ()
	{
		unref();
		m_raw = other.m_raw;
		ref();
	}

	void operator=(const ValueType &value) throw ()
	{
		unref();
		m_raw = Byteorder::Adapt(RawAccess::Extract(value));
		ref();
	}

	bool operator==(const Portable &other) const throw ()
	{
		return m_raw == other.m_raw;
	}

	bool operator!=(const Portable &other) const throw ()
	{
		return m_raw != other.m_raw;
	}

	bool operator==(const ValueType &value) const throw ()
	{
		return id() == RawAccess::Extract(value);
	}

	bool operator!=(const ValueType &value) const throw ()
	{
		return id() != RawAccess::Extract(value);
	}

	operator ValueType() const throw ()
	{
		return RawAccess::Materialize(id());
	}

	Pointer operator->() const throw ()
	{
		return Pointer(operator ValueType());
	}

	RawType id() const throw ()
	{
		return Byteorder::Adapt(m_raw);
	}

private:
	void ref() throw ()
	{
		RawAccess::Ref(id());
	}

	void unref() throw ()
	{
		RawAccess::Unref(id());
	}

	RawType m_raw;

} CONCRETE_PACKED;

} // namespace

#endif
