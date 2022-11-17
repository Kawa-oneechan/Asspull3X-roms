#include "ass.h"
extern IBios* interface;

extern "C"
{

//Needed for global constructors, for whatever reason.
void* __dso_handle = (void*) &__dso_handle;

void abort()
{
	printf("aborted!");
	while(true);
}

void* operator new(size_t sz)
{
	if (sz == 0) ++sz;
	return malloc(sz);
}

void* operator new[](size_t sz)
{
	return operator new(sz);
}

void operator delete(void* ptr)
{
	free(ptr);
}

void operator delete(void* ptr, unsigned long size)
{
	free(ptr);
}

void operator delete[](void* ptr)
{
	free(ptr);
}

void operator delete[](void* ptr, unsigned long size)
{
	free(ptr);
}

}
