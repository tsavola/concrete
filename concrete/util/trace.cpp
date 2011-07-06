/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifdef CONCRETE_TRACE

#include "trace.hpp"

#include <cstdint>

#include <concrete/arena-access.hpp>
#include <concrete/arena.hpp>
#include <concrete/pointer.hpp>
#include <concrete/util/byteorder.hpp>

namespace concrete {

void TraceData(const void *data) throw ()
{
	TraceData(data, Arena::AllocationSize(data));
}

void TraceData(const void *void_data, size_t size) throw ()
{
	assert(void_data);

	const uint32_t *word_data = reinterpret_cast<const uint32_t *> (void_data);
	unsigned int word_count = Arena::AlignedSize(size) / sizeof (word_data[0]);

	for (unsigned int i = 0; i < word_count; i++) {
		if ((i & 7) == 0)
			std::cout << boost::format("%5u:") % i;

		std::cout << boost::format(" %08lx") % PortByteorder(word_data[i]);

		if ((i & 7) == 7)
			std::cout << std::endl;
	}

	if (word_count & 7)
		std::cout << std::endl;
}

void TraceData(const Pointer &pointer) throw ()
{
	TraceData(Arena::Active().nonthrowing_access(pointer.address(), 0).data);
}

} // namespace

#endif
