/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

namespace concrete {

template <typename T>
typename PortableList<T>::NodeBase::Data *PortableList<T>::Data(const T &node)
{
	const Pointer &pointer = node;
	return pointer.cast<NodeBase>().data();
}

template <typename T>
PortableList<T>::operator bool() const throw ()
{
	return m_head->operator bool();
}

template <typename T>
bool PortableList<T>::operator!() const throw ()
{
	return !m_head;
}

template <typename T>
T PortableList<T>::head() const throw ()
{
	return m_head;
}

template <typename T>
void PortableList<T>::append(const T &node) throw ()
{
	if (*m_head) {
		T head = m_head;
		T tail = Data(head)->prev;

		Data(node)->next = head;
		Data(node)->prev = tail;

		Data(head)->prev = node;
		Data(tail)->next = node;
	} else {
		Data(node)->next = node;
		Data(node)->prev = node;

		m_head = node;
	}
}

template <typename T>
void PortableList<T>::remove(const T &node) throw ()
{
	T next = Data(node)->next;

	if (node == next) {
		m_head = T();
	} else {
		T prev = Data(node)->prev;

		Data(prev)->next = next;
		Data(next)->prev = prev;

		if (m_head == node)
			m_head = next;
	}
}

// CONCRETE_POINTER_IMPL_DATA
template <typename T>
typename ListNode<T>::Data *ListNode<T>::data() const
{
	return data_cast<Data>();
}

} // namespace
