/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef CONCRETE_INTERNALS_HPP
#define CONCRETE_INTERNALS_HPP

namespace concrete {

namespace internals {
	enum Serial {
		Object_repr,
		Object_str,

		LongObject_add,
		LongObject_repr,
		LongObject_str,

		StringObject_add,
		StringObject_repr,
		StringObject_str,

		BuiltinsModule_repr,
		BuiltinsModule_str,

		ConcreteModule_test,

		InternalCount
	};
}

} // namespace

#endif
