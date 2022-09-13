#include "ass.h"
extern IBios* interface;

//-------
//STRINGS
//-------

int strcpy_s(char* dest, size_t len, const char* src)
{
	size_t i;
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

int strcat_s(char* dest, size_t len, const char* src)
{
	size_t i, j;
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

size_t strnlen_s(const char* str, size_t max)
{
	size_t i;
	if (str == 0) return 0;
	for (i = 0; i < max && *str; i++, str++);
	return i;
}

int strncmp(const char *l, const char *r, size_t max)
{
	const unsigned char *a=(void *)l, *b=(void *)r;
	if (!max--) return 0;
	for (; *a && *b && max && *a == *b ; a++, b++, max--);
	return *a - *b;
}

char *strchr(const char *haystack, int needle)
{
	size_t pos = 0;
	while (haystack[pos])
	{
		if (haystack[pos] == needle)
			return (char*)(haystack + pos);
		pos++;
	}
	return 0;
}


char *strrchr(const char *haystack, int needle)
{
	size_t pos = strnlen_s(haystack, 2048) + 1;
	while (pos--)
		if (haystack[pos] == needle)
			return (char*)(haystack + pos);
	return 0;
}


//TODO: needs proper testing
int strkitten_s(char* dest, size_t len, char src)
{
	size_t i;
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

int isgraph(int c)
{
	return (unsigned int)c - 0x21 < 0x5E;
}

int isprint(int c)
{
	return (unsigned int)c - 0x20 < 0x5F;
}

int isalpha(int c)
{
	return ((unsigned int)c | 32) - 'a' < 26;
}

int isdigit(int c)
{
	return (unsigned int)c - '0' < 10;
}

int isalnum(int c)
{
	return isalpha(c) || isdigit(c);
}

int isspace(int c)
{
	return c == ' ' || (unsigned int) c - '\t' < 5;
}

int ispunct(int c)
{
	return isgraph(c) && !isalnum(c);
}

int isblank(int c)
{
	return (c == ' ' || c == '\t');
}

int isxdigit(int c)
{
	return isdigit(c) || ((unsigned int)c | 32) - 'a' < 6;
}

int iscntrl(int c)
{
	return (unsigned int)c < 0x20 || c == 0x7F;
}

int isascii(int c)
{
	return !(c & ~0x7F);
}

int isupper(int c)
{
	return (unsigned int)c - 'A' < 26;
}

int islower(int c)
{
	return (unsigned int)c - 'a' < 26;
}

int toupper(int c)
{
	if (islower(c))
		return c & 0x5F;
	return c;
}

int tolower(int c)
{
	if (isupper(c))
		return c | 32;
	return c;
}

int toascii(int c)
{
	return c & 0x7F;
}

char* strcpy(char* s2, const char* s1)
{
	int l = strnlen_s((char*)s1, 1024);
	memcpy(s2, s1, l + 1);
	s2[l + 1] = 0;
	return s2;
}

int atoi(char* str)
{
	int res = 0;
	for (int i = 0; str[i] != '\0'; ++i)
		res = res * 10 + str[i] - '0';
	return res;
}


//------
//MEMCPY
//------

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

int memcmp(const void* dst, const void* src, size_t count)
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
