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

#include <concrete/pointer.hpp>

namespace concrete {

class NoneObject;
class StringObject;
class TypeObject;
struct PortableObjectProtocol;

class Object: public Pointer {
	friend class NoneObject;
	friend class Pointer;
	friend class TypeObject;

public:
	struct RawAccess;

	static TypeObject Type();
	static Object New();

	Object() throw ();
	Object(const Object &other) throw ();
	~Object() throw ();

	void operator=(const Object &other) throw ();

	operator bool() const throw ();

	template <typename ObjectType> bool check() const;
	template <typename ObjectType> ObjectType require() const;

	TypeObject type() const;
	const PortableObjectProtocol *protocol() const;

	StringObject repr() const;
	StringObject str() const;

protected:
	struct Data;

	template <typename ObjectType, typename... Args>
	static ObjectType NewObject(Args... args);

	template <typename ObjectType, typename... Args>
	static ObjectType NewCustomSizeObject(size_t size, Args... args);

	explicit Object(unsigned int address) throw ();

private:
	static Data *NonthrowingData(const Pointer &object) throw ();

	static const Pointer &Referenced(const Pointer &object) throw ();

	static void Destroy(unsigned int address, Data *data) throw ();

	template <typename ObjectType>
	static void TypedDestroy(unsigned int address, Data *data) throw ();

	struct NoRefInit {};
	Object(const Pointer &pointer, NoRefInit) throw (): Pointer(pointer) {}

	void ref() const throw ();
	void unref() const throw ();

	Data *data() const;
	Data *nonthrowing_data() const throw ();
};

} // namespace

#endif
