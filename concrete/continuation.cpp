/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include "continuation.hpp"

#include <concrete/context.hpp>

namespace concrete {

Continuable::Continuable():
	m_state_id(0)
{
}

void Continuable::set_state(BlockId state_id) throw ()
{
	m_state_id = state_id;
}

} // namespace
