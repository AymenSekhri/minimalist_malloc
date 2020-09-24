#include <iostream>
#include "slab_allocator.h"

int main() {
	Slab<0x8, 0x20> slab_0x8;
	slab_0x8.init();
	printf("The size of slab is %x\n", sizeof(slab_0x8));

	void* addrsss1 = slab_0x8.alloc();
	void* addrsss2 = slab_0x8.alloc();
	printf("we got address %x \n", addrsss1);
	printf("we got address %x \n", addrsss2);
	slab_0x8.free(addrsss1);
	slab_0x8.free(addrsss2);
	addrsss1 = slab_0x8.alloc();
	addrsss2 = slab_0x8.alloc();
	addrsss2 = slab_0x8.alloc();
	printf("we got address %x \n", addrsss1);
	printf("we got address %x \n", addrsss2);

}
