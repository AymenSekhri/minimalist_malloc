#include "slab_allocator.h"

const size_t PAGE_SIZE = 0x2000;
static Slab<0x010, PAGE_SIZE> slab_0x10;
static Slab<0x020, PAGE_SIZE> slab_0x20;
static Slab<0x040, PAGE_SIZE> slab_0x40;
static Slab<0x080, PAGE_SIZE> slab_0x80;
static Slab<0x100, PAGE_SIZE> slab_0x100;
static Slab<0x200, PAGE_SIZE> slab_0x200;
static Slab<0x300, PAGE_SIZE> slab_0x300;

void free_from_slab(SlabHeader<1, 1> & generic_slab, void* address);

void custom_malloc_init() {
	slab_0x10.init();
	slab_0x20.init();
	slab_0x40.init();
	slab_0x80.init();
	slab_0x100.init();
	slab_0x200.init();
	slab_0x300.init();
}

void* custom_malloc(size_t size) {
	if (!size) {
		return nullptr;
	}
	if (size <= 0x10) {
		return slab_0x10.alloc();
	} else if (size <= 0x20) {
		return slab_0x20.alloc();
	} else if (size <= 0x40) {
		return slab_0x40.alloc();
	} else if (size <= 0x80) {
		return slab_0x80.alloc();
	} else if (size <= 0x100) {
		return slab_0x100.alloc();
	} else if (size <= 0x200) {
		return slab_0x200.alloc();
	} else if (size <= 0x500) {
		return slab_0x300.alloc();
	} else {
		return nullptr;
	}
}

void custom_free(void* address) {
	auto* block = reinterpret_cast<MemoryBlock<0>*>(uintptr_t(address) - sizeof(MemoryBlock<0>::slab_ptr));
	assert(block);
	assert(block->slab_ptr);
	auto* generic_slab = reinterpret_cast<SlabHeader<1, 1>*>(block->slab_ptr);
	assert(generic_slab->magic == MAGIC);
	free_from_slab(*generic_slab, address);
}

void free_from_slab(SlabHeader<1, 1> & generic_slab, void* address) {
	switch (generic_slab.size) {
		case 0x10:
		{
			auto* slab = reinterpret_cast<Slab<0x10, PAGE_SIZE>*>(&generic_slab);
			slab->free(address);
			break;
		}
		case 0x20:
		{
			auto* slab = reinterpret_cast<Slab<0x20, PAGE_SIZE>*>(&generic_slab);
			slab->free(address);
			break;
		}
		case 0x40:
		{
			auto* slab = reinterpret_cast<Slab<0x40, PAGE_SIZE>*>(&generic_slab);
			slab->free(address);
			break;
		}
		case 0x80:
		{
			auto* slab = reinterpret_cast<Slab<0x80, PAGE_SIZE>*>(&generic_slab);
			slab->free(address);
			break;
		}
		case 0x100:
		{
			auto* slab = reinterpret_cast<Slab<0x100, PAGE_SIZE>*>(&generic_slab);
			slab->free(address);
			break;
		}
		case 0x200:
		{
			auto* slab = reinterpret_cast<Slab<0x200, PAGE_SIZE>*>(&generic_slab);
			slab->free(address);
			break;
		}
		case 0x300:
		{
			auto* slab = reinterpret_cast<Slab<0x300, PAGE_SIZE>*>(&generic_slab);
			slab->free(address);
			break;
		}
		default:
			assert(false);
	}
}
