#pragma once
#include "bitmap.h"
#include <cstdint>
#include <Windows.h>
#include <iostream>

const static uint32_t MAGIC = 0x8CBEEFC8;

template<size_t slab_size, size_t memory_size> class Slab;
template<size_t slab_size, size_t memory_size, size_t max_blocks = memory_size / slab_size> struct SlabHeader {
	uint32_t magic;
	uint32_t size;
	size_t free_blocks;
	size_t next_fit_block;
	Slab<slab_size, memory_size>* prev, * next;
	Bitmap<max_blocks> mem_map;
};

template<size_t size>
struct MemoryBlock {
	uintptr_t slab_ptr;
	char data[size];
};

template<size_t slab_size, size_t memory_size> class Slab {
private:
	const static size_t MAX_HEADER_SIZE = sizeof(SlabHeader<slab_size, memory_size>);
	const static size_t MAX_BLOCKS = (memory_size - MAX_HEADER_SIZE) / sizeof(MemoryBlock<slab_size>);
	static_assert(memory_size > MAX_HEADER_SIZE);
	static_assert((sizeof(MemoryBlock<slab_size>) + MAX_HEADER_SIZE) <= memory_size);

	SlabHeader<slab_size, memory_size, MAX_BLOCKS> header;
	MemoryBlock<slab_size> blocks[MAX_BLOCKS];

	bool is_address_in_slab(void* address);
	void* alloc_in_current_slab(size_t block_index);
	void* alloc_in_new_slab();
	void free_from_current_slab(size_t block_index);
	void* request_memory_from_os(size_t size);
	void free_memory_to_os(void* addrss, size_t size);

public:


	void init(Slab* prev = nullptr);
	void* alloc();
	void free(void* address);
};

template<size_t slab_size, size_t memory_size>
void Slab<slab_size, memory_size>::init(Slab* prev) {
	header.magic = MAGIC;
	header.size = slab_size;
	header.prev = prev;
	header.next = nullptr;
	header.free_blocks = MAX_BLOCKS;
	header.next_fit_block = 0;
	header.mem_map.init();
}

template<size_t slab_size, size_t memory_size>
void* Slab<slab_size, memory_size>::alloc() {
	assert(header.magic == MAGIC);
	assert(header.size == slab_size);

	size_t block_index = -1;
	if (header.free_blocks &&
		((block_index = header.mem_map.find_unused(header.next_fit_block)) != BITMAP_NO_BITS_LEFT)) {
		return alloc_in_current_slab(block_index);
	} else if (header.next) {
		return header.next->alloc();
	} else {
		return alloc_in_new_slab();
	}

}

template<size_t slab_size, size_t memory_size>
void Slab<slab_size, memory_size>::free(void* address) {
	assert(header.magic == MAGIC);
	assert(header.size == slab_size);
	assert(is_address_in_slab(address));

	size_t block_index = (uintptr_t(address) - uintptr_t(blocks)) / sizeof(MemoryBlock<slab_size>);
	assert(header.mem_map.check_used(block_index));
	free_from_current_slab(block_index);
}

template<size_t slab_size, size_t memory_size>
bool Slab<slab_size, memory_size>::is_address_in_slab(void* address) {
	if ((address >= blocks) && (address <= &blocks[MAX_BLOCKS - 1].data[slab_size - 1])) {
		return true;
	} else {
		return false;
	}
	return true;
}

template<size_t slab_size, size_t memory_size>
void* Slab<slab_size, memory_size>::alloc_in_new_slab() {
	Slab* new_slab = static_cast<Slab*>(request_memory_from_os(sizeof(Slab)));
	if (!new_slab) {
		return nullptr;
	}
	new_slab->init(this);
	header.next = new_slab;
	return new_slab->alloc();
}

template<size_t slab_size, size_t memory_size>
void* Slab<slab_size, memory_size>::alloc_in_current_slab(size_t block_index) {
	header.mem_map.set_used(block_index);
	header.next_fit_block = (block_index + 1) % MAX_BLOCKS;
	header.free_blocks--;
	blocks[block_index].slab_ptr = uintptr_t(this);
	return static_cast<void*>(blocks[block_index].data);
}

template<size_t slab_size, size_t memory_size>
void Slab<slab_size, memory_size>::free_from_current_slab(size_t block_index) {
	header.mem_map.set_unused(block_index);
	header.next_fit_block = block_index;
	header.free_blocks++;

	if ((header.free_blocks == 0) && (header.prev)) {
		//slab is empty, and it's not the first;
		header.prev->header.next = nullptr;
		free_memory_to_os(this, sizeof(Slab));
		//The slab committed suicide, don't ever use it again!
	}
}

template<size_t slab_size, size_t memory_size>
void* Slab<slab_size, memory_size>::request_memory_from_os(size_t size) {
	//system dependent function, returns aligned memory region.
	void* address = VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
	if (!address) {
		printf("error: 0x%X\n", GetLastError());
		assert(false);
	}
	return address;
}

template<size_t slab_size, size_t memory_size>
void Slab<slab_size, memory_size>::free_memory_to_os(void* addrss, size_t size) {
	//system dependent function, returns aligned memory region.
	VirtualFree(addrss, size, MEM_FREE);
}