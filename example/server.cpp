/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <exception>
#include <iostream>

#include <concrete/context.hpp>
#include <concrete/execute.hpp>

#include "common.hpp"

using namespace concrete;

namespace example {

static void handle(int fd)
{
	Context context(load_context(fd));
	ContextScope scope(context);

	try {
		Executor::Snapshot executor_snapshot;

		if (!load_executor(executor_snapshot, fd))
			return;

		Executor executor(executor_snapshot);

		while (executor.execute())
			context.poll_events();

	} catch (const Exception &e) {
		std::cerr << type_name(e) << ": " << e.what() << std::endl;

	} catch (const std::exception &e) {
		std::cerr << "exception: " << e.what() << std::endl;
	}
}

static int main()
{
	handle(0);
	return 0;
}

} // namespace

int main()
{
	return example::main();
}
