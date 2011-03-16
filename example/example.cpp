#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <typeinfo>

#include <cxxabi.h>

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

template <typename T>
static std::string type_name(const T &object)
{
	auto c_str = abi::__cxa_demangle(typeid (object).name(), 0, 0, 0);
	std::string str = c_str;
	std::free(c_str);
	return str;
}

static int main()
{
	Context context;
	ContextScope scope(context);

	try {
		Executor executor(load_code());

		while (executor.execute())
			context.arena().check_error();

	} catch (const Exception &e) {
		std::cerr << type_name(e) << ": " << e.what() << "\n";
		return 1;
	}

	return 0;
}

} // namespace

int main()
{
	return example::main();
}
