		#pragma once
		#include "bitmap.h"
		#include <cstdint>
		#include <Windows.h>

		template<size_t slab_size, size_t memory_size> class Slab;
		template<size_t slab_size, size_t memory_size, size_t max_blocks = memory_size / slab_size> struct SlabHeader {
			Slab<slab_size, memory_size>* prev, * next;
			Bitmap<max_blocks> mem_map;
			size_t free_blocks;
			size_t next_fit_block;
		};

		template<size_t slab_size, size_t memory_size> class Slab {
		private:
			const static size_t MAX_HEADER_SIZE = sizeof(SlabHeader<slab_size, memory_size>);
			const static size_t MAX_BLOCKS = (memory_size - MAX_HEADER_SIZE) / slab_size;
			static_assert(memory_size > MAX_HEADER_SIZE);
			static_assert((slab_size + MAX_HEADER_SIZE) <= memory_size);

			SlabHeader<slab_size, memory_size, MAX_BLOCKS> header;
			char blocks[MAX_BLOCKS][slab_size];

			bool is_address_in_slab(void* address);
			void* alloc_in_current_slab(size_t block_index);
			void* alloc_in_new_slab();
			void free_from_current_slab(size_t block_index);
			void free_from_next_slab(void* address);
			void* request_memory_from_os(size_t size);
			void free_memory_to_os(void* addrss, size_t size);

		public:
			void init(Slab* prev = nullptr);
			void* alloc();
			void free(void* address);
		};

		template<size_t slab_size, size_t memory_size>
		void Slab<slab_size, memory_size>::init(Slab* prev) {
			header.prev = prev;
			header.next = nullptr;
			header.free_blocks = MAX_BLOCKS;
			header.next_fit_block = 0;
			header.mem_map.init();
		}

		template<size_t slab_size, size_t memory_size>
		void* Slab<slab_size, memory_size>::alloc() {
			size_t block_index = -1;
			if (header.free_blocks &&
				((block_index = header.mem_map.find_unused(header.next_fit_block)) != BITMAP_NO_BITS_LEFT)) {
				return alloc_in_current_slab(block_index);
			} else {
				return alloc_in_new_slab();
			}

		}

		template<size_t slab_size, size_t memory_size>
		void Slab<slab_size, memory_size>::free(void* address) {
			if (is_address_in_slab(address) == false) {
				return free_from_next_slab(address);
			}
			size_t block_index = (uintptr_t(address) - uintptr_t(blocks)) / slab_size;
			assert(header.mem_map.check_used(block_index));
			free_from_current_slab(block_index);
		}

		template<size_t slab_size, size_t memory_size>
		bool Slab<slab_size, memory_size>::is_address_in_slab(void* address) {
			if ((address >= blocks) && (address <= &blocks[MAX_BLOCKS - 1][slab_size - 1])) {
				return true;
			} else {
				return false;
			}
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
			return static_cast<void*>(blocks[block_index]);
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
		void Slab<slab_size, memory_size>::free_from_next_slab(void* address) {
			if (header.next) {//if there is another slab in the list check on it too.
				header.next->free(address);
				return;
			} else {
				//address doesn't belong any slab.
				return;
			}
		}

		template<size_t slab_size, size_t memory_size>
		void* Slab<slab_size, memory_size>::request_memory_from_os(size_t size) {
			//system dependent function, returns aligned memory region.
			return VirtualAlloc(0, size, MEM_COMMIT, PAGE_READWRITE);
		}

		template<size_t slab_size, size_t memory_size>
		void Slab<slab_size, memory_size>::free_memory_to_os(void* addrss, size_t size) {
			//system dependent function, returns aligned memory region.
			VirtualFree(addrss, size, MEM_FREE);
		}