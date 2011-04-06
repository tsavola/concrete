/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_UTIL_ID_HPP
#define CONCRETE_UTIL_ID_HPP

#include <cassert>
#include <cstddef>

#include <concrete/util/portable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

template <typename T>
struct IdOps {
	static const T &Load(const T &x) throw ()
	{
		return x;
	}

	static const T &Store(const T &x) throw ()
	{
		return x;
	}
};

template <typename T>
struct PortableIdOps {
	static T Load(T x) throw ()
	{
		return PortableOps<T, sizeof (T)>::Load(x);
	}

	static T Store(T x) throw ()
	{
		return PortableOps<T, sizeof (T)>::Store(x);
	}
};

template <typename T, typename Ops>
class IdLogic {
	// use nullptr_t in the future
	typedef decltype (NULL) Null;

public:
	static IdLogic New(T value) throw ()
	{
		assert(value != NoRawValue());

		IdLogic id;
		id.m_raw_value = Ops::Store(value);
		return id;
	}

	IdLogic() throw ():
		m_raw_value(NoRawValue())
	{
	}

	IdLogic(const IdLogic &other) throw ():
		m_raw_value(other.m_raw_value)
	{
	}

	template <typename OtherOps>
	IdLogic(const IdLogic<T, OtherOps> &other) throw ():
		m_raw_value(Ops::Store(other.value()))
	{
	}

	IdLogic(Null null) throw ():
		m_raw_value(NoRawValue())
	{
		assert(!null);
	}

	IdLogic &operator=(const IdLogic &other) throw ()
	{
		m_raw_value = other.m_raw_value;
		return *this;
	}

	template <typename OtherOps>
	IdLogic &operator=(const IdLogic<T, OtherOps> &other) throw ()
	{
		m_raw_value = Ops::Store(other.value());
		return *this;
	}

	IdLogic &operator=(Null null) throw ()
	{
		assert(!null);

		m_raw_value = NoRawValue();
		return *this;
	}

	bool operator==(const IdLogic &other) const throw ()
	{
		return m_raw_value == other.m_raw_value;
	}

	bool operator!=(const IdLogic &other) const throw ()
	{
		return m_raw_value != other.m_raw_value;
	}

	template <typename OtherOps>
	bool operator==(const IdLogic<T, OtherOps> &other) const throw ()
	{
		return value() == other.value();
	}

	template <typename OtherOps>
	bool operator!=(const IdLogic<T, OtherOps> &other) const throw ()
	{
		return value() != other.value();
	}

	bool operator==(Null null) const throw ()
	{
		assert(!null);

		return m_raw_value == NoRawValue();
	}

	bool operator!=(Null null) const throw ()
	{
		assert(!null);

		return m_raw_value != NoRawValue();
	}

	operator bool() const throw ()
	{
		return m_raw_value != NoRawValue();
	}

	T value() const throw ()
	{
		return Ops::Load(m_raw_value);
	}

private:
	static T NoRawValue() throw ()
	{
		return Ops::Store(~T(0));
	}

	T m_raw_value;

} CONCRETE_PACKED;

} // namespace

#endif
