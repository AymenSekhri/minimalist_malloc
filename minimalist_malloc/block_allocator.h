#pragma once
#include "bitmap.h"
#include <cstdint>
#include <Windows.h>

void* request_memory_from_os(size_t size) {
	//system dependent function, returns aligned memory region.
	return VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
}

void free_memory_from_os(void* addrss, size_t size) {
	//system dependent function, returns aligned memory region.
	VirtualFree(addrss, size, MEM_FREE);
}

template<size_t SLAB_SIZE, size_t MEM_SIZE> class Slab {
private:
	struct SlabHeader {
		Slab<SLAB_SIZE, MEM_SIZE>* prev, * next;
		Bitmap<SLAB_SIZE> mem_map;
		size_t free_blocks;
		size_t next_fit_block;
	};
	const static size_t MAX_BLOCKS = (MEM_SIZE - sizeof(SlabHeader)) / SLAB_SIZE;

	SlabHeader header;
	char blocks[MAX_BLOCKS][SLAB_SIZE];

	static_assert(MEM_SIZE > sizeof(SlabHeader));
	static_assert((SLAB_SIZE + sizeof(SlabHeader)) <= MEM_SIZE);

public:
	void init(Slab* prev = nullptr) {
		header.prev = prev;
		header.next = nullptr;
		header.free_blocks = MAX_BLOCKS;
		header.next_fit_block = 0;
		header.mem_map.init();
	}

	void* alloc() {
		size_t block_index = -1;
		if (header.free_blocks && ((block_index = header.mem_map.find_unused(header.next_fit_block)) != BITMAP_NO_BITS_LEFT)) {
			header.mem_map.set_used(block_index);
			header.next_fit_block = block_index + 1;
			header.free_blocks--;
			return static_cast<void*>(blocks[block_index]);
		} else {
			Slab* new_slab = static_cast<Slab*>(request_memory_from_os(sizeof(Slab)));
			new_slab->init(this);
			header.next = new_slab;
			return new_slab->alloc();
		}

	}

	void free(void* address) {
		if ((address < blocks) || (address > & blocks[MAX_BLOCKS - 1][SLAB_SIZE - 1])) {
			assert(false);//address doesn't belong to this slab.
			return;
		}
		size_t block_index = (uintptr_t(address) - uintptr_t(blocks)) / SLAB_SIZE;
		header.mem_map.set_unused(block_index);
		header.next_fit_block = block_index;
		header.free_blocks++;

		if ((header.free_blocks == 0) && (header.prev)) {
			//slab is empty, and it's not the first;
			header.prev->header.next = nullptr;
			free_memory_from_os(this, sizeof(Slab));
			//The slab committed suicide, don't ever use it again!
		}

	}


};