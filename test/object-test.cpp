#include <cstdint>

#include <gtest/gtest.h>

#include <concrete/block.hpp>
#include <concrete/objects/object.hpp>
#include <concrete/util/portable.hpp>

using namespace concrete;

namespace {

TEST(portable_object, size)
{
	EXPECT_EQ(sizeof (BlockId), sizeof (PortableObject));
}

TEST(portable_type_object, size)
{
	EXPECT_EQ(sizeof (BlockId), sizeof (PortableTypeObject));
}

TEST(object_block, size)
{
	EXPECT_EQ(sizeof (PortableObject) + sizeof (portable<int32_t>), sizeof (ObjectBlock) - sizeof (Block));
}

class context : public ::testing::Test {
protected:
	void SetUp()
	{
		Context temp_context;
		{
			ContextScope scope(temp_context);
		}

		context.activate();
	}

	void TearDown()
	{
		context.deactivate();
	}

	Context context;
};

} // namespace
