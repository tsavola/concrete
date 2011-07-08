/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cstddef>
#include <memory>

#include <concrete/arena.hpp>

namespace concrete {

template <typename T, typename... Args>
ResourceSlot ResourceManager::new_resource(Args... args)
{
	std::unique_ptr<T> resource(ResourceCreate<T>::New(args...));
	auto slot = add_resource(resource.get());
	resource.release();

	return slot;
}

template <typename T>
T *ResourceManager::resource_cast(ResourceSlot slot) const
{
	auto base = find_resource(slot);
	if (base == NULL)
		throw IntegrityError(slot.address());

	auto derived = dynamic_cast<T *> (base);
	if (derived == NULL)
		throw IntegrityError(slot.address());

	return derived;
}

template <typename T>
PortableResource<T>::~PortableResource() throw ()
{
	destroy();
}

template <typename T>
template <typename... Args>
void PortableResource<T>::create(Args... args)
{
	destroy();
	m_slot = ResourceManager::Active().new_resource<T>(args...);
}

template <typename T>
void PortableResource<T>::destroy() throw ()
{
	if (*m_slot) {
		ResourceManager::Active().destroy_resource(m_slot);
		m_slot = ResourceSlot();
	}
}

template <typename T>
bool PortableResource<T>::is_lost() const throw ()
{
	return ResourceManager::Active().is_resource_lost(m_slot);
}

template <typename T>
PortableResource<T>::operator bool() const throw ()
{
	return m_slot->operator bool();
}

template <typename T>
bool PortableResource<T>::operator!() const throw ()
{
	return m_slot->operator!();
}

template <typename T>
T *PortableResource<T>::operator*() const throw ()
{
	return ResourceManager::Active().resource_cast<T>(m_slot);
}

template <typename T>
T *PortableResource<T>::operator->() const throw ()
{
	return operator*();
}

template <typename T>
template <typename... Args>
T *ResourceCreate<T>::New(Args... args)
{
	return new T(args...);
}

} // namespace
