/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_OBJECTS_OBJECT_PARTIAL_HPP
#define CONCRETE_OBJECTS_OBJECT_PARTIAL_HPP

#include <concrete/block.hpp>

namespace concrete {

class StringObject;
class TypeObject;

class Object {
public:
	struct RawAccess {
		typedef BlockId RawType;

		static BlockId Default() throw ();

		template <typename ObjectType>
		static BlockId Extract(const ObjectType &object) throw ();

		template <typename ObjectType>
		static ObjectType Materialize(BlockId id) throw ();

		template <typename ObjectType>
		static ObjectType Materialize(BlockId id, ObjectType *) throw ();

		static void Ref(BlockId id) throw ();
		static void Unref(BlockId id) throw ();
	};

	struct Protocol;

	static TypeObject Type();
	static Object New();

	Object();
	Object(const Object &other) throw ();
	~Object() throw ();

	Object &operator=(const Object &other) throw ();

	bool operator==(const Object &other) const throw ();
	bool operator!=(const Object &other) const throw ();

	BlockId id() const throw ();

	template <typename ObjectType> bool check() const;
	template <typename ObjectType> ObjectType cast() const;
	template <typename ObjectType> ObjectType require() const;

	TypeObject type() const;
	const Protocol &protocol() const;

	Object add(const Object &) const;
	StringObject repr() const;
	StringObject str() const;

protected:
	struct Content;

	explicit Object(BlockId id) throw ();

	template <typename ContentType> ContentType *content_pointer() const;

private:
	static void Destroy(Content *content, BlockId id) throw ();

	void ref() const throw ();
	void unref() const throw ();
	Content *content() const;

	BlockId m_id;
};

} // namespace

#endif
