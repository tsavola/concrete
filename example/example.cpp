#include <fstream>
#include <iostream>

#include <boost/scoped_array.hpp>

#include <concrete/context.hpp>
#include <concrete/exception.hpp>
#include <concrete/execute.hpp>
#include <concrete/objects/code.hpp>

using namespace concrete;

static CodeObject load_example_code()
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

int main()
{
	Context context;
	ContextScope scope(context);

	try {
		Executor executor(load_example_code());
		while (executor.execute())
			;
	} catch (const Exception &e) {
		std::cerr << e.what() << "\n";
		return 1;
	}

	return 0;
}
