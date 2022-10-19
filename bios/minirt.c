#include "../ass.h"

int abs(int i)
{
	if (i < 0) return -i;
	return i;
}

void* memcpy(void* dst, const void* src, size_t count)
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

void* memset(void* dst, int val, size_t count)
{
	void* start = dst;
	while (count--)
	{
		*(char *)dst = (char)val;
		dst = (char *)dst + 1;
	}
	return(start);
}

int strncmp(const char *l, const char *r, size_t max)
{
	const unsigned char *a=(void *)l, *b=(void *)r;
	if (!max--) return 0;
	for (; *a && *b && max && *a == *b ; a++, b++, max--);
	return *a - *b;
}

char* strcpy(char* s2, const char* s1)
{
	int l;
	const char* src = s1;
	for (l = 0; l < 1024 && *src; l++, src++);
	memcpy(s2, s1, l + 1);
	s2[l + 1] = 0;
	return s2;
}

