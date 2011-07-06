/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <cassert>

#include <concrete/util/trace.hpp>

namespace concrete {

template <typename ImplType, typename... ImplParams>
Object Continuation::Call(Continuation &cont,
                          Stage stage,
                          const TupleObject *args,
                          const DictObject *kwargs,
                          ImplParams... impl_params)
{
	ImplType impl = cont.cast<ImplType>();
	bool done = false;
	Object result;

	switch (stage) {
	case Initiate:
		assert(!impl);
		assert(args && kwargs);

		impl = NewPointer<ImplType>();
		done = impl.initiate(result, *args, *kwargs, impl_params...);
		break;

	case Resume:
		assert(impl);

		done = impl.resume(result, impl_params...);
		break;

	case Release:
		assert(impl);

		done = true;
		break;
	}

	if (done)
		DestroyPointer(impl);

	cont = impl;

	return result;
}

} // namespace
