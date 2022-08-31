#include "ass.h"
extern IBios* interface;

extern "C"
{

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
