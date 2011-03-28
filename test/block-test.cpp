#include <gtest/gtest.h>

#include <concrete/arena.hpp>
#include <concrete/block.hpp>

using namespace concrete;

namespace {

TEST(block, size)
{
#ifdef CONCRETE_BLOCK_MAGIC
	EXPECT_EQ(8, sizeof (Block));
#else
	EXPECT_EQ(4, sizeof (Block));
#endif
}

TEST(arena, alloc_min)
{
	Arena arena;

	BlockId id = arena.alloc_block(sizeof (Block)).id;
	Block *ptr = arena.block_pointer(id, sizeof (Block));
	EXPECT_TRUE(ptr);
	EXPECT_EQ(ptr->block_size(), sizeof (Block));
	// leak the block
}

TEST(arena, alloc_big)
{
	Arena arena;

	auto ret = arena.alloc_block(131072);
	Block *ptr = ret.ptr;
	EXPECT_TRUE(ptr);
	EXPECT_EQ(ptr->block_size(), 131072);
	arena.free_block(ret.id);
}

TEST(arena, alloc_odd_multi)
{
	Arena arena;

	for (int pass = 0; pass < 2; pass++) {
		BlockId block[10];

		for (int i = 0; i < 10; i++) {
			int size = sizeof (Block) + i * 4 + (((i + pass) & 1) ? 1 : 3);
			block[i] = arena.alloc_block(size).id;
			Block *ptr = arena.block_pointer(block[i], sizeof (Block));
			EXPECT_TRUE(ptr);
			EXPECT_EQ(uintptr_t(ptr) & 3, 0);
			EXPECT_EQ(ptr->block_size(), size);

			for (int prev = 0; prev < i; prev++)
				EXPECT_NE(block[i], block[prev]);
		}

		for (int i = 3; i < 10; i++) // leak the first 3
			arena.free_block(block[i]);
	}
}

} // namespace
