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

	auto code = CodeObject::Load(buf.get(), size);
	Context::Active().arena().check_error();

	return code;
}

static ContextSnapshot load_snapshot(int fd)
{
	ContextSnapshot snapshot;
	ssize_t length;

	length = snapshot.head_size();

	if (read(fd, snapshot.head_ptr(), length) != length)
		throw std::runtime_error("read snapshot head");

	length = snapshot.data_size();
	auto data = std::malloc(length);

	if (read(fd, data, length) != length) {
		std::free(data);
		throw std::runtime_error("read snapshot data");
	}

	snapshot.init_data(data);

	return snapshot;
}

static void save_snapshot(const ContextSnapshot &snapshot, int fd)
{
	ssize_t length;

	length = snapshot.head_size();

	if (write(fd, snapshot.head_ptr(), length) != length)
		throw std::runtime_error("write snapshot head");

	length = snapshot.data_size();

	if (write(fd, snapshot.data_ptr(), length) != length)
		throw std::runtime_error("write snapshot data");
}

template <typename T>
static std::string type_name(const T &object)
{
	auto c_str = abi::__cxa_demangle(typeid (object).name(), 0, 0, 0);
	std::string str = c_str;
	std::free(c_str);
	return str;
}

static void work(Context &context)
{
	ContextScope scope(context);

	std::cerr << "EXAMPLE: " << getpid() << ": execution begin" << std::endl;

	try {
		Executor executor(load_code());

		while (executor.execute())
			context.arena().check_error();

	} catch (const Exception &e) {
		std::cerr << "EXAMPLE: " << type_name(e) << ": " << e.what() << "\n";
	}

	std::cerr << "EXAMPLE: " << getpid() << ": execution end" << std::endl;
}

static void dump(const ContextSnapshot &snapshot)
{
	std::cerr << "EXAMPLE: " << getpid() << ": arena size = " << snapshot.data_size() << std::endl;

	boost::crc_32_type crc32;
	crc32.process_bytes(snapshot.data_ptr(), snapshot.data_size());

	std::cerr << "EXAMPLE: " << getpid() << ": arena checksum = " << crc32.checksum() << std::endl;
}

static void save(const ContextSnapshot &snapshot, int output_fd)
{
	dump(snapshot);

	if (output_fd >= 0)
		save_snapshot(snapshot, output_fd);
}

static void initial_work(int input_fd, int output_fd)
{
	Context context;

	save(context.snapshot(), output_fd);
}

static void secondary_work(int input_fd, int output_fd)
{
	Context context(load_snapshot(input_fd));

	dump(context.snapshot());
	work(context);
	save(context.snapshot(), output_fd);
}

static pid_t start_worker(void func(int, int), int input_fd, int output_fd)
{
	pid_t pid = fork();

	if (pid == 0) {
		func(input_fd, output_fd);
		std::exit(0);
	}

	return pid;
}

static int main()
{
	int fd12[2];
	int fd23[2];

	pipe(fd12);
	pipe(fd23);

	std::cerr << "EXAMPLE: starting workers" << std::endl;

	pid_t p1 = start_worker(initial_work, -1, fd12[1]);
	pid_t p2 = start_worker(secondary_work, fd12[0], fd23[1]);
	pid_t p3 = start_worker(secondary_work, fd23[0], -1);

	waitpid(p1, NULL, 0);
	waitpid(p2, NULL, 0);
	waitpid(p3, NULL, 0);

	std::cerr << "EXAMPLE: workers exited" << std::endl;

	return 0;
}

} // namespace

int main()
{
	return example::main();
}
