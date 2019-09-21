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

//#define BURELLE
#ifdef BURELLE
//Okay!
typedef struct s_block
{
	int size;
	void* next;
	void* prev;
	int free;
	void* ptr;
	char data[];
} s_block;

//#define BLOCK_SIZE sizeof(s_block)
#define BLOCK_SIZE 20

void* base = NULL;

void split_block(s_block* b, int s)
{
	s_block* new;
	new = (s_block*)(b->data + s);
	new->size = b->size - s - BLOCK_SIZE;
	new->next = (s_block*)b->next;
	new->prev = b;
	new->free = 1;
	new->ptr = new->data;
	b->size = s;
	b->next = new;
	if (new->next)
		((s_block*)new->next)->prev = new;
}

s_block* extend_heap(s_block* last, int size)
{
	s_block* b = (s_block*)sbrk(0);
	int sb = (int)sbrk(BLOCK_SIZE + 2);
	if (sb < 0) return NULL; //sbrk fails, go to die
	b->size = size;
	b->next = NULL;
	b->prev = last;
	b->ptr = b->data;
	if (last)
		last->next = b;
	b->free = 0;
	return b;
}

s_block* get_block(void *p)
{
	char *tmp = p;
	return (p = tmp -= BLOCK_SIZE);
}

int valid_addr(void *p)
{
	if (base)
		if (p > base && p < (void*)sbrk(0))
			return (p == (get_block(p))->ptr);
	return 0;
}

s_block* find_block(s_block* last, int size)
{
	s_block* b = base;
	while (b && !(b->free && b->size >= size))
	{
		last = b;
		b = b->next;
	}
	return b;
}

s_block* fusion(s_block* b)
{
	if (b->next && ((s_block*)b->next)->free)
	{
		b->size += BLOCK_SIZE +((s_block*)b->next)->size;
		b->next = ((s_block*)b->next)->next;
		if (b->next) ((s_block*)b->next)->prev = b;
	}
	return b;
}

void* malloc(unsigned int size)
{
	s_block* b;
	s_block* last;
	int s = ALIGN4(size);
	if (base > 0)
	{
		//First, find a block.
		last = (s_block*)base;
		b = find_block(last, s);
		if (b)
		{
			//Can we split?
			if ((b->size - s) >= (BLOCK_SIZE + 4))
				split_block(b, s);
			b->free = 0;
		}
		else
		{
			//No fitting block, extend the heap.
			b = extend_heap(last, s);
			if (!b) return NULL;
		}
	}
	else
	{
		//First call!
		base = (void*)0x03000000;
		heap = base;
		b = extend_heap(NULL, s);
		if (!b) return NULL;
		base = b;
	}
	return b->data;
}

void free(void *p)
{
	s_block* b;
	if (valid_addr(p))
	{
		b = get_block(p);
		b->free = 1;
		//Fuse with previous if possible...
		if (b->prev && ((s_block*)b->prev)->free)
			b = fusion(b->prev);
		//...and with next.
		if (b->next)
			fusion(b);
		else
		{
			//Free the end of the heap.
			if (b->prev)
				((s_block*)b->prev)->next = NULL;
			else
				base = NULL;
			brk((char*)b);
		}
	}
}

void* calloc(int number, int size)
{
	int* new;
	int s4, i;
	new = malloc(number * size);
	if (new)
	{
		s4 = ALIGN4(number * size) << 2;
		for (i =0; i < s4; i++)
			new[i] = 0;
	}
	return new;
}
#else

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
#endif

//#endif //BIOS
