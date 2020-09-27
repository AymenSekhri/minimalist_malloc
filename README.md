# Minimalist Memory Allocator
This project is a simple slab allocator that uses mix of Next-fit search and Segregated-list search algorithms.<br>
It consists of multiple `Slab`s of different memory sizes, a `Slab` is header plus contagious memory blocks of the same size. Whenever a `Slab` runs out of memory, it allocate a new `Slab` with the same size, and links it to the current `Slab`. Free blocks in a `Slab` are tracked using a bitmap in `Slab`'s header. <br>


## Why ?
It's just for educational proposes, and I'm using it in my [operating system](https://github.com/AymenSekhri/CyanOS).

## Speed Benchmarks
To test this allocator against compilers' implementation, I have ran few comparing it with MSVC compiler's malloc.<br>
#### System Information
- OS: Windows 10 Pro 2004
- CPU: i7 6700hq 2.6GHz
- Compiler: Visual Studio 19 MSVC (32bit) with /O2 optimization 


### Test 1: 100 Allocations before freeing memory
The code:
```
const int num = 100;
void* addresses[num];
for (size_t i = 0; i < num; i++) {
    addresses[i]= malloc(allocation_size);
    if (!addresses[i]) {
        assert(false);
    }
    memset(addresses[i], 0xCC, allocation_size);
}
for (size_t i = 0; i < num; i++) {
    free(addresses[i]);
}
```

| Allocation size | slab allocator duration (ns) | MSVC's malloc duration (ns) | % Difference |
| --------------- | ---------------------------- | --------------------------- | ------------ |
| 0x10            | 10,373                       | 37,163                      | +358%        |
| 0x40            | 12,517                       | 35,162                      | +281%        |
| 0x70            | 19,818                       | 36,634                      | +185%        |
| 0x90            | 16,109                       | 40,723                      | +253%        |

### Test 2: 600 Allocations before freeing memory
The difference between this test and the previous one is that it does 600 memory allocations before starting freeing memory, which will test the slab's flexibility when there are no memory blocks available.<br>
The code:
```
const int num = 600;
void* addresses[num];
for (size_t i = 0; i < num; i++) {
    addresses[i]= malloc(allocation_size);
    if (!addresses[i]) {
        assert(false);
    }
    memset(addresses[i], 0xCC, allocation_size);
}
for (size_t i = 0; i < num; i++) {
    free(addresses[i]);
}
```

| Allocation size | slab allocator duration (ns) | MSVC's malloc duration (ns) | % difference |
| --------------- | ---------------------------- | --------------------------- | ------------ |
| 0x10            | 129,589                      | 230,356                     | +178%        |
| 0x40            | 218,750                      | 190,484                     | -115%        |
| 0x70            | 358,836                      | 195,425                     | -184%        |
| 0x90            | 140,633                      | 201,083                     | +143%        |

### Comments
Although theses tests might not reflect the performance of a real application, but it's the closest I can think of. We can notice that the Slab allocator outperforms MSVC's malloc in all memory allocation sizes for 100 allocations. Though, it struggles when we increase the allocations number, that's because it needs to perform a system call to allocate more memory for the slab, in the other hand MSVC's malloc seems to have some kind of optimization to know ahead of time that it needs more memory and allocate it before the user request it.

## References

http://dmitrysoshnikov.com/compilers/writing-a-memory-allocator/
