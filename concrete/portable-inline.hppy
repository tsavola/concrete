/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

namespace concrete {

/*
 * Raw access
 */

template <typename T>
typename PortableRawAccess<T>::RawType PortableRawAccess<T>::InitRef() throw ()
{
	return T::RawAccess::InitRef();
}

template <typename T>
void PortableRawAccess<T>::Ref(RawType raw) throw ()
{
	T::RawAccess::Ref(raw);
}

template <typename T>
void PortableRawAccess<T>::Unref(RawType raw) throw ()
{
	T::RawAccess::Unref(raw);
}

template <typename T>
typename PortableRawAccess<T>::RawType PortableRawAccess<T>::Extract(const T &value) throw ()
{
	return T::RawAccess::Extract(value);
}

template <typename T>
typename PortableRawAccess<T>::RawType PortableRawAccess<T>::ExtractRef(const T &value) throw ()
{
	return T::RawAccess::ExtractRef(value);
}

template <typename T>
T PortableRawAccess<T>::Materialize(RawType raw) throw ()
{
	T *type = 0;
	return T::RawAccess::Materialize(raw, type);
}

/*
 * Primitive access
 */

template <typename T, typename PrimitiveType>
PrimitiveType PortablePrimitiveAccess<T, PrimitiveType>::InitRef() throw ()
{
	return 0;
}

template <typename T, typename PrimitiveType>
PrimitiveType PortablePrimitiveAccess<T, PrimitiveType>::Extract(T value) throw ()
{
	return PrimitiveType(value);
}

template <typename T, typename PrimitiveType>
PrimitiveType PortablePrimitiveAccess<T, PrimitiveType>::ExtractRef(T value) throw ()
{
	return PrimitiveType(value);
}

template <typename T, typename PrimitiveType>
T PortablePrimitiveAccess<T, PrimitiveType>::Materialize(PrimitiveType raw) throw ()
{
	return T(raw);
}

/*
 * Pointer
 */

template <typename T>
Portable<T>::Pointer::Pointer(const T &value) throw ():
	m_value(value)
{
}

template <typename T>
Portable<T>::Pointer::Pointer(const Pointer &other) throw ():
	m_value(other.m_value)
{
}

template <typename T>
T *Portable<T>::Pointer::operator->() throw ()
{
	return &m_value;
}

/*
 * Portable
 */

template <typename T>
Portable<T>::Portable() throw ():
	m_raw(Byteorder::Adapt(RawAccess::InitRef()))
{
}

template <typename T>
Portable<T>::Portable(const Portable &other) throw ():
	m_raw(other.m_raw)
{
	ref();
}

template <typename T>
Portable<T>::Portable(const T &value) throw ():
	m_raw(Byteorder::Adapt(RawAccess::ExtractRef(value)))
{
}

template <typename T>
Portable<T>::~Portable() throw ()
{
	unref();
}

template <typename T>
void Portable<T>::operator=(const Portable &other) throw ()
{
	unref();
	m_raw = other.m_raw;
	ref();
}

template <typename T>
void Portable<T>::operator=(const T &value) throw ()
{
	unref();
	m_raw = Byteorder::Adapt(RawAccess::ExtractRef(value));
}

template <typename T>
bool Portable<T>::operator==(const Portable &other) const throw ()
{
	return m_raw == other.m_raw;
}

template <typename T>
bool Portable<T>::operator!=(const Portable &other) const throw ()
{
	return m_raw != other.m_raw;
}

template <typename T>
bool Portable<T>::operator==(const T &value) const throw ()
{
	return native_raw() == RawAccess::Extract(value);
}

template <typename T>
bool Portable<T>::operator!=(const T &value) const throw ()
{
	return native_raw() != RawAccess::Extract(value);
}

template <typename T>
Portable<T>::operator T() const throw ()
{
	return RawAccess::Materialize(native_raw());
}

template <typename T>
bool Portable<T>::operator!() const throw ()
{
	return !operator T();
}

template <typename T>
T Portable<T>::operator*() const throw ()
{
	return operator T();
}

template <typename T>
typename Portable<T>::Pointer Portable<T>::operator->() const throw ()
{
	return Pointer(operator*());
}

template <typename T>
void Portable<T>::ref() throw ()
{
	RawAccess::Ref(native_raw());
}

template <typename T>
void Portable<T>::unref() throw ()
{
	RawAccess::Unref(native_raw());
}

template <typename T>
typename Portable<T>::RawType Portable<T>::native_raw() const throw ()
{
	return Byteorder::Adapt(m_raw);
}

} // namespace
