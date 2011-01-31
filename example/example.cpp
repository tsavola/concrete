#include <concrete/context.hpp>
#include <concrete/execute.hpp>
#include <concrete/load.hpp>

using namespace concrete;

int main()
{
	Context::Init();

	Context context;
	ContextScope scope(context);
	CodeObject code = load_code_file(EXAMPLE_BYTECODE);
	Executor executor(code);

	while (executor.execute())
		;

	return 0;
}
