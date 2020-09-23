
#include "block_allocator.h"
const size_t PAGE_SIZE = 0x1000;
static Slab<0x010, PAGE_SIZE> slab_0x10;
static Slab<0x020, PAGE_SIZE> slab_0x20;
static Slab<0x040, PAGE_SIZE> slab_0x40;
static Slab<0x060, PAGE_SIZE> slab_0x60;
static Slab<0x100, PAGE_SIZE> slab_0x100;
static Slab<0x200, PAGE_SIZE> slab_0x200;
static Slab<0x500, PAGE_SIZE> slab_0x500;
static Slab<0x1000, PAGE_SIZE> slab_variable;

void* malloc(size_t size) {
	if (size < 0x10) {
		return slab_0x10.alloc();
	} else if (size < 0x20) {
		return slab_0x10.alloc();
	} else if (size < 0x40) {
		return slab_0x40.alloc();
	} else if (size < 0x60) {
		return slab_0x60.alloc();
	} else if (size < 0x100) {
		return slab_0x100.alloc();
	} else if (size < 0x200) {
		return slab_0x200.alloc();
	} else if (size < 0x500) {
		return slab_0x500.alloc();
	} else {
		return slab_variable.alloc();
	}
}

void free(void* address) {
	slab_0x10.free(address);
	slab_0x20.free(address);
	slab_0x40.free(address);
	slab_0x60.free(address);
	slab_0x100.free(address);
	slab_0x200.free(address);
	slab_0x500.free(address);
}