#include "ass.h"
extern IBios* interface;

#if !defined NULL
#define NULL (void*)0
#endif

//-------
//STRINGS
//-------

int strcpy_s(char* dest, int len, const char* src)
{
	int i;
	if (!dest) return 1;
	if (len == 0) return 1;
	if (!src)
	{
		dest[0] = '\0';
		return 1;
	}
	for (i = 0; i < len; i++)
		if ((dest[i] = src[i]) == '\0') return 0;
	dest[0] = '\0';
	return 2;
}

int strcat_s(char* dest, int len, const char* src)
{
	int i, j;
	if (!dest) return 1;
	if (len == 0) return 1;
	if (!src)
	{
		dest[0] = '\0';
		return 1;
	}
	for (i = 0; i < len; i++)
	{
		if (dest[i] == '\0')
		{
			for (j = 0; (j + i) < len; j++)
				if ((dest[j + i] = src[j]) == '\0') return 0;
		}
	}
	dest[0] = '\0';
	return 2;
}

int strnlen_s(const char* str, int max)
{
	int i;
	if (str == 0) return 0;
	for (i = 0; i < max && *str; i++, str++);
	return i;
}

//TODO: needs proper testing
int strkitten_s(char* dest, int len, char src)
{
	int i;
	if (!dest) return 1;
	if (len == 0) return 1;
	if (!src)
	{
		dest[0] = '\0';
		return 1;
	}
	i = strnlen_s(dest, len);
	//just about here
	if (i < len)
	{
		dest[i++] = src;
		dest[i++] = '\0';
		return 0;
	}

	dest[0] = '\0';
	return 2;
}

//------
//MEMCPY
//------
void* memcpy(void* dst, const void* src, int count)
{
	void* ret = dst;
	while (count--)
	{
		*(char*)dst = *(char*)src;
		dst = (char*)dst + 1;
		src = (char*)src + 1;
	}
	return(ret);
}

void* memset(void* dst, int val, int count)
{
	void* start = dst;
	while (count--)
	{
		*(char *)dst = (char)val;
		dst = (char *)dst + 1;
	}
	return(start);
}

int memcmp(const void* dst, const void* src, int count)
{
	int r = 0;
	const char* d = (const char*)dst;
	const char* s = (const char*)src;
	while (count-- && r == 0)
	{
		r = *d++ - *s++;
	}
	return r;
}

/*#ifndef BIOS
#warning USING MALLOC WRAPPERS
void* malloc(unsigned int size) { return interface->malloc(size); }
void free(void *p) { interface->free(p); }
void* calloc(int number, int size) { return interface->calloc(number, size); }
#else
#warning USING ACTUAL FULL MALLOC
*/
//-----------
//MALLOC/FREE
//-----------
#define ALIGN4(x) (((((x)-1)>>2)<<2)+4)
extern int  __HEAP_START; //0x03000000 presumed.

void* heap = (void*)0x03000000; //0;
void* sbrk(int incr)
{
	if (incr == 0) return heap;
	void *prev_heap;
	if (heap == 0)
	{
		heap = (void*)0x03000000; //(char*)&__HEAP_START;
	}
	prev_heap = heap;

	if (((unsigned int)heap + incr) >= 0x04000000)
	{
		return prev_heap;
	}

	heap += incr;
	return prev_heap; //(void*)ALIGN4((unsigned int)prev_heap);
}
void* brk(void* new_heap)
{
	void* prev_heap = heap;
	heap = new_heap;
	return prev_heap;
}

typedef struct malloc_block_meta {
	int size;
	struct malloc_block_meta* next;
	int free;
} malloc_block_meta;

#define MALLOC_META_SIZE sizeof(struct malloc_block_meta)

void* malloc_global_base = NULL;

malloc_block_meta* malloc_find_free_block(struct malloc_block_meta** last, int size)
{
	malloc_block_meta* current = malloc_global_base;
	while (current && !(current->free && current->size >= size))
	{
		*last = current;
		current = current->next;
	}
	return current;
}

struct malloc_block_meta* malloc_request_space(struct malloc_block_meta* last, int size)
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

void* malloc(unsigned int size)
{
	malloc_block_meta* block;
	size = ALIGN4(size);
	if (size <= 0) return NULL;
	if (malloc_global_base == 0) //First call.
	{
		malloc_global_base = (void*)0x03000000;
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

malloc_block_meta* malloc_get_block_ptr(void* ptr)
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

void* realloc(void* ptr, int size)
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

void* calloc(int nelem, int elsize)
{
	int size = nelem * elsize; //TODO: check for overflow.
	void* ptr = malloc(size);
	memset(ptr, 0, size);
	return ptr;
}

//#endif //BIOS
