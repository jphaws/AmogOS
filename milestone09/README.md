# Milestone 9

## Heap Allocator (kmalloc)
The next milestone is to implement a kernel heap allocator / deallocator. Example function declarations are:

```C
extern void kfree(void *addr);
extern void *kmalloc(size_t size);
```

The details of your allocator are up to you, however I suggest using something on the simpler side to begin with. If you deterime later it is too inefficient you can always replace it.

You are not required to mimic my heap allocator implementation, but I'll briefly describe it here to help get you started in your design. I used a modified [fixed size block allocator.](https://en.wikipedia.org/wiki/Memory_management#FIXED-SIZE) Specifically I have a limited number of fixed size block pools: 32, 64, 128, 512, 1024, and 2048. Each pool is tracked with a simple data structure containing the usable size of each block, the number of available free blocks in the pool, and a pointer to the first available free block:

```C
struct KmallocPool {
   int max_size;
   int avail;
   struct FreeList *head;
};
```

All the free blocks within a pool are kept in a linked list of O(1) insertion and removal with a LIFO policy:

```C
struct FreeList {
   struct FreeList *next;
};
```

When a piece of memory is allocated a small header is placed **before** the address returned by kmalloc(). This header provide kfree() the extra information it needs to return the block of memory to the correct pool:

```C
struct KmallocExtra {
   struct KmallocPool *pool;
   size_t size;
};
```

To allocate memory, kmalloc() searches for the pool with the best fit for the requested size, taking into account the memory required for the KmallocExtra structure. If there is no suitable pool enough page frames with contiguous virtual addresses are allocated to cover the request.

If the request fits in one of the pools the next free block from that pool is used. If there are no more free blocks in the pool a new virtual page is allocated for the pool, all blocks in that new page are added to the linked list, and then the first free block is used.

Prior to returning the KmallocExtra information is added and the returned address is incremented by sizeof(struct KmallocExtra).

To free memory, kfree() first decrements the pointer by sizeof(struct KmallocExtra) to access the metadata. If the allocation came from a pool the block is added back into the pool's free list. If the allocation was from a pool (raw virtual pages) the size of the allocation is used to free all allocated virtual pages.

Note that in this scheme virtual addresses used for the large allocations are never reused, however the associated page frames are.

As a point of reference my kernel heap implementation is 140 lines of C.
