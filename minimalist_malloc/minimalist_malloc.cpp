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
		duration += duration_cast<microseconds>(stop - start).count();
	}
	return duration / iterations;
}

void benchmark_allocation_size(size_t allocation_size) {

	auto my_malloc_time = calculate_execution_time([&]() {
		for (size_t i = 0; i < 10; i++) {
			void* addrsss = custom_malloc(allocation_size);
			memset(addrsss, 0xCC, allocation_size);
			custom_free(addrsss);
		}
	});

	auto malloc_time = calculate_execution_time([&]() {
		for (size_t i = 0; i < 10; i++) {
			void* addrsss = malloc(allocation_size);
			memset(addrsss, 0xCC, allocation_size);
			free(addrsss);
		}
	});
	printf("For size 0x%X\n", allocation_size);
	std::cout << "my malloc time: " << my_malloc_time << std::endl;
	std::cout << "real malloc time: " << malloc_time << std::endl;
	if (my_malloc_time > malloc_time) {
		printf("my malloc is %f times slower than the real malloc\n", (double)my_malloc_time / malloc_time);
	} else {
		printf("my malloc is %.2f times faster than the real malloc\n", (double)malloc_time / my_malloc_time);
	}
	printf("------------------------------------------------\n");
}

int main() {
	custom_malloc_init();

	for (size_t i = 1; i < 20; i++) {
		benchmark_allocation_size(0x10 * i);
	}
}

