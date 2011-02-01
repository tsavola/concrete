/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "exception.hpp"

#include <concrete/objects/object.hpp>
#include <concrete/objects/type.hpp>

namespace concrete {

const char *Exception::what() const throw ()
{
	return m_args.repr().data();
}

const char *TypeError::what() const throw ()
{
	return m_args.cast<TypeObject>().name().data();
}

} // namespace
