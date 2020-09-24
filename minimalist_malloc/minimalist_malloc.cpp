#include <iostream>
#include "slab_allocator.h"
#include "malloc.h"

int main() {
	custom_malloc_init();
	void* addrsss1 = custom_malloc(4);
	void* addrsss2 = custom_malloc(20);
	printf("we got address %x \n", addrsss1);
	printf("we got address %x \n", addrsss2);
	custom_free(addrsss1);
	custom_free(addrsss2);
	addrsss1 = custom_malloc(0x145);
	addrsss2 = custom_malloc(0x145);
	printf("we got address %x \n", addrsss1);
	printf("we got address %x \n", addrsss2);

}
