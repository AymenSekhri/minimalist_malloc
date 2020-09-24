# Minimalist Memory Allocator
This project is a simple slab allocator that uses mix of Next-fit search and Segregated-list search.<br>
It consists of multiple `Slab`s of different memory sizes, a `Slab` is header plus contagious memory blocks of the same size. Whenever a `Slab` runs out of memory, it allocate a new `Slab` with the same size, and link it to the current `Slab`. Free blocks in a `Slab` are tracked using a bitmap in `Slab`'s header. <br>
`free` function will ask all the `Slab`s to try to mark certain block as free if it exits in that `Slab`, If a certain block was the last used block in a `Slab`, that it will be freed from memory.

## Why ?
It's just for educational proposes only, and I'm using it in my [operating system](https://github.com/AymenSekhri/CyanOS).

## References

http://dmitrysoshnikov.com/compilers/writing-a-memory-allocator/