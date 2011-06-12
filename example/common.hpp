/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#ifndef EXAMPLE_COMMON_HPP
#define EXAMPLE_COMMON_HPP

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>

#include <sys/types.h>
#include <unistd.h>

#include <cxxabi.h>

#include <concrete/context.hpp>
#include <concrete/execute.hpp>

namespace example {

using namespace concrete;

template <typename T>
static std::string type_name(const T &object)
{
	auto c_str = abi::__cxa_demangle(typeid (object).name(), 0, 0, 0);
	std::string str = c_str;
	std::free(c_str);
	return str;
}

static inline Context::Snapshot load_context(int fd)
{
	Context::Snapshot snapshot;
	ssize_t length;

	length = snapshot.head_size();

	if (read(fd, snapshot.head_ptr(), length) != length)
		throw std::runtime_error("read context snapshot head");

	length = snapshot.data_size();
	auto data = std::malloc(length);

	if (read(fd, data, length) != length) {
		std::free(data);
		throw std::runtime_error("read context snapshot data");
	}

	snapshot.init_data(data);

	return snapshot;
}

static inline bool load_executor(Executor::Snapshot &snapshot, int fd)
{
	ssize_t length = read(fd, snapshot.ptr(), snapshot.size());

	if (length == 0)
		return false;

	if (length == ssize_t(snapshot.size()))
		return true;

	throw std::runtime_error("read executor snapshot");
}

} // namespace

#endif
