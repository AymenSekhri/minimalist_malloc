#include <iostream>
#include "slab_allocator.h"
#include "malloc.h"
#include <chrono> 
using namespace std::chrono;

template<typename Func>
long long calculate_execution_time(Func funcion) {
	const size_t iterations = 1000;
	long long duration = 0;
	for (size_t i = 0; i < iterations; i++) {
		auto start = high_resolution_clock::now();
		funcion();
		auto stop = high_resolution_clock::now();
		duration += duration_cast<nanoseconds>(stop - start).count();
	}
	return duration / iterations;
}

void benchmark_allocation_size(size_t allocation_size) {
	const int num = 100;
	void* addresses[num];

	auto my_malloc_time = calculate_execution_time([&]() {
		for (size_t i = 0; i < num; i++) {
			addresses[i]= custom_malloc(allocation_size);
			if (!addresses[i]) {
				assert(false);
			}
			memset(addresses[i], 0xCC, allocation_size);
		}
		for (size_t i = 0; i < num; i++) {
			custom_free(addresses[i]);
		}
	});

	auto malloc_time = calculate_execution_time([&]() {
		for (size_t i = 0; i < num; i++) {
			addresses[i] = malloc(allocation_size);
			memset(addresses[i], 0xCC, allocation_size);
		}
		for (size_t i = 0; i < num; i++) {
			free(addresses[i]);
		}
	});
	printf("For size 0x%X\n", allocation_size);
	std::cout << "my malloc time: " << my_malloc_time << std::endl;
	std::cout << "real malloc time: " << malloc_time << std::endl;
	if (my_malloc_time > malloc_time) {
		printf("custom_malloc is %.2f times slower than malloc\n", (double)my_malloc_time / malloc_time);
	} else {
		printf("custom_malloc is %.2f times faster than malloc\n", (double)malloc_time / my_malloc_time);
	}
	printf("------------------------------------------------\n");
}

int main() {
	custom_malloc_init();

	for (size_t i = 1; i < 10; i++) {
		benchmark_allocation_size(0x10 * i);
	}
}

