/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

namespace concrete {

template <typename Member>
void TypeObject::set(Member member, const Object &value) const
{
	protocol()->*member = value;
}

template <typename ObjectType>
bool TypeCheck<ObjectType>::operator()(const TypeObject &type)
{
	return type == ObjectType::Type();
}

} // namespace
