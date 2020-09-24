#include "pch.h"
#include "../minimalist_malloc/malloc.h"
#include "../minimalist_malloc/slab_allocator.h"



TEST(SlubTests, TestName) {
	static Slab<0x010, 0x200> slab;
	slab.init();
	EXPECT_EQ(sizeof(slab), 0x10);
	slab.alloc();
}