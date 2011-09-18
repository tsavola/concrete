/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_LIST_HPP
#define CONCRETE_LIST_HPP

#include <concrete/portable.hpp>
#include <concrete/util/noncopyable.hpp>
#include <concrete/util/packed.hpp>

namespace concrete {

template <typename ImplType> class ListNode;

template <typename NodeType>
class PortableList: Noncopyable {
public:
	operator bool() const throw ();
	bool operator!() const throw ();

	NodeType head() const throw ();
	void append(const NodeType &node) throw ();
	void remove(const NodeType &node) throw ();

private:
	typedef ListNode<NodeType> NodeBase;

	static typename NodeBase::Data *Data(const NodeType &node);

	Portable<NodeType> m_head;
} CONCRETE_PACKED;

template <typename ImplType>
class ListNode: public Pointer {
	CONCRETE_POINTER_DECL_COMMON(ListNode, Pointer)
	CONCRETE_POINTER_DECL_CONSTRUCT(ListNode)
	CONCRETE_POINTER_DECL_DATA(ListNode)

	friend class PortableList<ImplType>;

protected:
	struct Data: Noncopyable {
		Portable<ImplType> prev;
		Portable<ImplType> next;
	} CONCRETE_PACKED;
};

} // namespace

#include "list-inline.hpp"

#endif
