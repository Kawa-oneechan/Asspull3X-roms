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

size_t strlen(const char* str)
{
	size_t i;
	if (str == 0) return 0;
	for (i = 0; *str; i++, str++);
	return i;
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

char* strdup(const char* s1)
{
	int l = strlen((char*)s1);
	char* s2 = (char*)malloc(l + 1);
	memcpy(s2, s1, l + 1);
	s2[l + 1] = 0;
	return s2;
}

int strcat(char* dest, const char* src)
{
	size_t i, j;
	if (!dest) return 1;
	if (!src)
	{
		dest[0] = '\0';
		return 1;
	}
	for (i = 0; ; i++)
	{
		if (dest[i] == '\0')
		{
			for (j = 0; ; j++)
				if ((dest[j + i] = src[j]) == '\0') return 0;
		}
	}
	dest[0] = '\0';
	return 2;
}

int strkitten(char* dest, char src)
{
	size_t i;
	if (!dest) return 1;
	i = strlen(dest);
	dest[i++] = src;
	dest[i++] = '\0';
	return 0;
}

int getchar()
{
	uint16_t key = 0;
	while (1)
	{
		key = INP_KEYIN & 0xFF;
		if (key > 0)
			break;
	}
	while (1)
	{
		if ((INP_KEYIN & 0xFF) == 0)
			break;
	}
	if (INP_KEYSHIFT & 1)
		key += 128;
	return interface->locale.sctoasc[key];
}

