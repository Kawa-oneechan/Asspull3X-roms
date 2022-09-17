#include "ass.h"
extern IBios* interface;

//-----------
//MALLOC/FREE
//-----------

#define ALIGN4(x) (((((x)-1)>>2)<<2)+4)
extern int __HEAP_START; //0x01100000 presumed.

void* heap = (void*)0x01100000; //0;

void* sbrk(int incr)
{
	if (incr == 0) return heap;
	void *prev_heap;
	if (heap == 0)
	{
		heap = (void*)0x01100000; //(char*)&__HEAP_START;
	}
	prev_heap = heap;

	if (((uint32_t)heap + incr) >= 0x01400000)
	{
		return prev_heap;
	}

	heap += incr;
	return prev_heap; //(void*)ALIGN4((uint32_t)prev_heap);
}

void* brk(void* new_heap)
{
	void* prev_heap = heap;
	heap = new_heap;
	return prev_heap;
}

typedef struct malloc_block_meta {
	uint32_t size;
	struct malloc_block_meta* next;
	int free;
} malloc_block_meta;

#define MALLOC_META_SIZE sizeof(struct malloc_block_meta)

void* malloc_global_base = NULL;

static malloc_block_meta* malloc_find_free_block(struct malloc_block_meta** last, size_t size)
{
	malloc_block_meta* current = malloc_global_base;
	while (current && !(current->free && current->size >= size))
	{
		*last = current;
		current = current->next;
	}
	return current;
}

static struct malloc_block_meta* malloc_request_space(struct malloc_block_meta* last, size_t size)
{
	malloc_block_meta *block;
	block = (malloc_block_meta*)sbrk(0);
	void* request = sbrk(size + MALLOC_META_SIZE);
	//assert((void*)block == request); //Not thread safe.
	if (request == (void*)-1) return NULL; //sbrk failed.
	if (last) last->next = block; //NULL on first request.
	block->size = size;
	block->next = NULL;
	block->free = 0;
	return block;
}

void* malloc(size_t size)
{
	malloc_block_meta* block;
	size = ALIGN4(size);
	if (size <= 0) return NULL;
	if (malloc_global_base == 0) //First call.
	{
		malloc_global_base = (void*)0x01100000;
		heap = malloc_global_base;
		block = malloc_request_space(NULL, size);
		if (!block) return NULL;
		malloc_global_base = block;
	} else
	{
		malloc_block_meta* last = malloc_global_base;
		block = malloc_find_free_block(&last, size);
		if (!block) //Failed to find free block.
		{
			block = malloc_request_space(last, size);
			if (!block) return NULL;
		} else // Found free block
		{
			//TODO: consider splitting block here.
			block->free = 0;
		}
	}

	return (block+1);
}

static malloc_block_meta* malloc_get_block_ptr(void* ptr)
{
	return (malloc_block_meta*)ptr - 1;
}

void free(void* ptr)
{
	if (!ptr) return;
	//TODO: consider merging blocks once splitting blocks is implemented.
	malloc_block_meta* block_ptr = malloc_get_block_ptr(ptr);
	//assert(block_ptr->free == 0);
	block_ptr->free = 1;
}

void* realloc(void* ptr, size_t size)
{
	if (!ptr) //NULL ptr. realloc should act like malloc.
		return malloc(size);

	malloc_block_meta* block_ptr = malloc_get_block_ptr(ptr);
	if (block_ptr->size >= size) //We have enough space. Could free some once we implement split.
		return ptr;

	// Need to really realloc. Malloc new space and free old space.
	// Then copy old data to new space.
	void* new_ptr;
	new_ptr = malloc(size);
	if (!new_ptr) return NULL; //TODO: set errno on failure.

	memcpy(new_ptr, ptr, block_ptr->size);
	free(ptr);
	return new_ptr;
}

void* calloc(size_t nelem, size_t elsize)
{
	size_t size = nelem * elsize; //TODO: check for overflow.
	void* ptr = malloc(size);
	memset(ptr, 0, size);
	return ptr;
}

char* strdup(const char* s1)
{
	size_t l = 0; //strnlen_s((char*)s1, 1024);
	char* s2 = (char*)s1;
	while (*s2) l++;
	s2 = malloc(l + 1);
	memcpy(s2, s1, l + 1);
	s2[l + 1] = 0;
	return s2;
}
