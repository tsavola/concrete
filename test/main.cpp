#include <gtest/gtest.h>

#include <concrete/context.hpp>

int main(int argc, char **argv)
{
	concrete::Context::Init();

	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
