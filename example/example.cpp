#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <typeinfo>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cxxabi.h>

#include <boost/crc.hpp>
#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>

#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/execute.hpp>
#include <concrete/objects/code.hpp>

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

static ContextSnapshot load_context(int fd)
{
	ContextSnapshot snapshot;
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

static void save_context(const ContextSnapshot &snapshot, int fd)
{
	ssize_t length;

	length = snapshot.head_size();

	if (write(fd, snapshot.head_ptr(), length) != length)
		throw std::runtime_error("write context snapshot head");

	length = snapshot.data_size();

	if (write(fd, snapshot.data_ptr(), length) != length)
		throw std::runtime_error("write context snapshot data");
}

static bool load_executor(ExecutorSnapshot &snapshot, int fd)
{
	ssize_t length = read(fd, snapshot.ptr(), snapshot.size());

	if (length == 0)
		return false;

	if (length == ssize_t(snapshot.size()))
		return true;

	throw std::runtime_error("read executor snapshot");
}

static void save_executor(const ExecutorSnapshot &snapshot, int fd)
{
	ssize_t length = snapshot.size();

	if (write(fd, snapshot.ptr(), length) != length)
		throw std::runtime_error("write executor snapshot");
}

template <typename T>
static std::string type_name(const T &object)
{
	auto c_str = abi::__cxa_demangle(typeid (object).name(), 0, 0, 0);
	std::string str = c_str;
	std::free(c_str);
	return str;
}

static void dump(const ContextSnapshot &snapshot)
{
	std::cerr << "EXAMPLE: " << getpid() << ": arena size = " << snapshot.data_size() << std::endl;

	boost::crc_32_type crc32;
	crc32.process_bytes(snapshot.data_ptr(), snapshot.data_size());

	std::cerr << "EXAMPLE: " << getpid() << ": arena checksum = " << crc32.checksum() << std::endl;
}

static void dump(const ExecutorSnapshot &snapshot)
{
	std::cerr << "EXAMPLE: " << getpid() << ": executor snapshot = "
	          << int32_t(snapshot.initial_frame_id.offset()) << " "
	          << int32_t(snapshot.current_frame_id.offset()) << std::endl;
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
		ExecutorSnapshot executor_snapshot;
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

		for (int i = 0; count < 0 || i < count; i++)
			if (!executor->execute())
				break;

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
