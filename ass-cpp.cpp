#include "ass.h"
extern IBios* interface;

extern "C"
{

//Needed for namespace-level instances,
//but crt0 doesn't construct them yet.
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

void operator delete[](void* ptr)
{
	free(ptr);
}

}
