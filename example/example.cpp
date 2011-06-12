/*
 * Copyright (c) 2011  Timo Savola
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 */

#include <exception>
#include <fstream>
#include <iostream>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/crc.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>

#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/execute.hpp>
#include <concrete/objects/code.hpp>

#include "common.hpp"

using namespace concrete;

namespace example {

static CodeObject load_code()
{
	std::ifstream stream;

	stream.exceptions(std::ios::failbit | std::ios::badbit);
	stream.open(EXAMPLE_BYTECODE);

	stream.seekg(0, std::ios::end);
	auto size = stream.tellg();
	stream.seekg(0, std::ios::beg);

	boost::scoped_array<char> buf(new char[size]);

	stream.read(buf.get(), size);
	stream.close();

	return CodeObject::Load(buf.get(), size);
}

static void save_context(const Context::Snapshot &snapshot, int fd)
{
	ssize_t length;

	length = snapshot.head_size();

	if (write(fd, snapshot.head_ptr(), length) != length)
		throw std::runtime_error("write context snapshot head");

	length = snapshot.data_size();

	if (write(fd, snapshot.data_ptr(), length) != length)
		throw std::runtime_error("write context snapshot data");
}

static void save_executor(const Executor::Snapshot &snapshot, int fd)
{
	ssize_t length = snapshot.size();

	if (write(fd, snapshot.ptr(), length) != length)
		throw std::runtime_error("write executor snapshot");
}

static void dump(const Context::Snapshot &snapshot)
{
	std::cerr << "EXAMPLE: " << getpid() << ": arena size = " << snapshot.data_size() << std::endl;

	boost::crc_32_type crc32;
	crc32.process_bytes(snapshot.data_ptr(), snapshot.data_size());

	std::cerr << "EXAMPLE: " << getpid() << ": arena checksum = " << crc32.checksum() << std::endl;
}

static void dump(const Executor::Snapshot &snapshot)
{
	std::cerr << "EXAMPLE: " << getpid() << ": executor snapshot = "
	          << int32_t(snapshot.initial_frame_id) << " "
	          << int32_t(snapshot.current_frame_id) << std::endl;
}

static void initial_work(int count, int input_fd, int output_fd)
{
	Context context;

	dump(context.snapshot());
	save_context(context.snapshot(), output_fd);
}

static void secondary_work(int count, int input_fd, int output_fd)
{
	Context context(load_context(input_fd));
	ContextScope scope(context);

	dump(context.snapshot());

	try {
		Executor::Snapshot executor_snapshot;
		boost::scoped_ptr<Executor> executor;

		if (load_executor(executor_snapshot, input_fd)) {
			std::cerr << "EXAMPLE: " << getpid() << ": executor snapshot found" << std::endl;

			dump(executor_snapshot);

			executor.reset(new Executor(executor_snapshot));
		} else {
			std::cerr << "EXAMPLE: " << getpid() << ": loading bytecode" << std::endl;

			executor.reset(new Executor(load_code()));
		}

		std::cerr << "EXAMPLE: " << getpid() << ": execution begin" << std::endl;

		for (int i = 0; count < 0 || i < count; i++) {
			if (!executor->execute())
				break;

			context.poll_events();
		}

		std::cerr << "EXAMPLE: " << getpid() << ": execution end" << std::endl;

		dump(context.snapshot());
		dump(executor->snapshot());

		if (output_fd >= 0) {
			save_context(context.snapshot(), output_fd);
			save_executor(executor->snapshot(), output_fd);
		}
	} catch (const Exception &e) {
		std::cerr << "EXAMPLE: " << getpid() << ": " << type_name(e) << ": " << e.what()
		          << std::endl;
	} catch (const std::exception &e) {
		std::cerr << "EXAMPLE: " << getpid() << ": exception: " << e.what() << std::endl;
	}
}

static pid_t start_worker(void func(int, int, int), int count, int input_fd, int output_fd,
                          int fd1, int fd2, int fd3)
{
	pid_t pid = fork();

	if (pid == 0) {
		if (fd1 >= 0) close(fd1);
		if (fd2 >= 0) close(fd2);
		if (fd3 >= 0) close(fd3);

		func(count, input_fd, output_fd);
		std::exit(0);
	}

	return pid;
}

static void wait_worker(pid_t pid)
{
	int status = -1;
	waitpid(pid, &status, 0);

	std::cerr << "EXAMPLE: worker " << pid << " exited with status " << status << std::endl;
}

static int main()
{
	int fd12[2];
	int fd23[2];

	pipe(fd12);
	pipe(fd23);

	std::cerr << "EXAMPLE: starting workers" << std::endl;

	pid_t p1 = start_worker(initial_work,    0,      -1, fd12[1], fd12[0], fd23[0], fd23[1]);
	pid_t p2 = start_worker(secondary_work, 60, fd12[0], fd23[1], fd12[1], fd23[0],      -1);
	pid_t p3 = start_worker(secondary_work, -1, fd23[0],      -1, fd12[0], fd12[1], fd23[1]);

	close(fd12[0]);
	close(fd12[1]);
	close(fd23[0]);
	close(fd23[1]);

	wait_worker(p1);
	wait_worker(p2);
	wait_worker(p3);

	return 0;
}

} // namespace

int main()
{
	return example::main();
}
