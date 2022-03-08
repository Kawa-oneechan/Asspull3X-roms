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

/*#ifndef BIOS
#warning USING MALLOC WRAPPERS
void* malloc(size_t size) { return interface->malloc(size); }
void free(void *p) { interface->free(p); }
void* calloc(int number, int size) { return interface->calloc(number, size); }
#else
#warning USING ACTUAL FULL MALLOC
*/
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

malloc_block_meta* malloc_find_free_block(struct malloc_block_meta** last, size_t size)
{
	malloc_block_meta* current = malloc_global_base;
	while (current && !(current->free && current->size >= size))
	{
		*last = current;
		current = current->next;
	}
	return current;
}

struct malloc_block_meta* malloc_request_space(struct malloc_block_meta* last, size_t size)
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


//----
//TIME
//----

#define DAYS_PER_YEAR 365 //number of days in a non-leap year
#define SECSPERMIN 60L
#define MINSPERHOUR 60L
#define HOURSPERDAY 24L
#define SECSPERHOUR (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY (SECSPERHOUR * HOURSPERDAY)
#define SECSPERYEAR (SECSPERDAY * DAYS_PER_YEAR)
#define DAYSPERWEEK 7
#define EPOCH_ADJUSTMENT_DAYS 719468L
#define ADJUSTED_EPOCH_YEAR 0 //year to which the adjustment was made
#define ADJUSTED_EPOCH_WDAY 3 //1st March of year 0 is Wednesday
#define DAYS_PER_ERA 146097L //there are 97 leap years in 400-year periods. ((400 - 97) * 365 + 97 * 366)
#define DAYS_PER_CENTURY 36524L //there are 24 leap years in 100-year periods. ((100 - 24) * 365 + 24 * 366)
#define DAYS_PER_4_YEARS (3 * 365 + 366) //there is one leap year every 4 years
#define DAYS_IN_JANUARY 31 //number of days in January
#define DAYS_IN_FEBRUARY 28 //number of days in non-leap February
#define YEARS_PER_ERA 400 //number of years per era
#define YEAR_BASE 1900
#define EPOCH_YEAR 1970
#define EPOCH_WDAY 4
#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

tm __gmtime_res;

char* asctime_r(const tm *timeptr, char* buf)
{
	static const char wday_name[][4] = {
		"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
	};
	static const char mon_name[][4] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};
	TEXT->Format(buf, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
		wday_name[timeptr->tm_wday],
		mon_name[timeptr->tm_mon],
		timeptr->tm_mday, timeptr->tm_hour,
		timeptr->tm_min, timeptr->tm_sec,
		YEAR_BASE + timeptr->tm_year);
	return buf;
}

char* asctime(const tm *timeptr)
{
	static char buf[32];
	return asctime_r(timeptr, buf);
}

tm* gmtime(const time_t* timer)
{
	tm* res = &__gmtime_res;
	int32_t days, rem;
	const time_t lcltime = *timer;
	int era, weekday, year;
	uint32_t erayear, yearday, month, day;
	uint32_t eraday;

	days = lcltime / SECSPERDAY + EPOCH_ADJUSTMENT_DAYS;
	rem = lcltime % SECSPERDAY;
	if (rem < 0)
	{
		rem += SECSPERDAY;
		--days;
	}

	//compute hour, min, and sec
	res->tm_hour = (int) (rem / SECSPERHOUR);
	rem %= SECSPERHOUR;
	res->tm_min = (int) (rem / SECSPERMIN);
	res->tm_sec = (int) (rem % SECSPERMIN);

	//compute day of week
	if ((weekday = ((ADJUSTED_EPOCH_WDAY + days) % DAYSPERWEEK)) < 0)
	weekday += DAYSPERWEEK;
	res->tm_wday = weekday;

	//compute year, month, day & day of year
	era = (days >= 0 ? days : days - (DAYS_PER_ERA - 1)) / DAYS_PER_ERA;
	eraday = days - era * DAYS_PER_ERA;	//[0, 146096]
	erayear = (eraday - eraday / (DAYS_PER_4_YEARS - 1) + eraday / DAYS_PER_CENTURY -
	eraday / (DAYS_PER_ERA - 1)) / 365;	//[0, 399]
	yearday = eraday - (DAYS_PER_YEAR * erayear + erayear / 4 - erayear / 100);	//[0, 365]
	month = (5 * yearday + 2) / 153;	//[0, 11]
	day = yearday - (153 * month + 2) / 5 + 1;	//[1, 31]
	month += month < 10 ? 2 : -10;
	year = ADJUSTED_EPOCH_YEAR + erayear + era * YEARS_PER_ERA + (month <= 1);

	res->tm_yday = yearday >= DAYS_PER_YEAR - DAYS_IN_JANUARY - DAYS_IN_FEBRUARY ?
		yearday - (DAYS_PER_YEAR - DAYS_IN_JANUARY - DAYS_IN_FEBRUARY) :
		yearday + DAYS_IN_JANUARY + DAYS_IN_FEBRUARY + isleap(erayear);
	res->tm_year = year - YEAR_BASE;
	res->tm_mon = month;
	res->tm_mday = day;

	res->tm_isdst = 0;

	return (res);
}

char* ctime_r(const time_t* timer, char* buf)
{
	return asctime_r(gmtime(timer), buf);
}

char* ctime(const time_t* timer)
{
	return asctime(gmtime(timer));
}

time_t mktime(tm* timeptr)
{
    //......wow.
    return timeptr->tm_sec
        + timeptr->tm_min * SECSPERMIN
        + timeptr->tm_hour * SECSPERHOUR
        + timeptr->tm_yday * SECSPERDAY
        + (timeptr->tm_year - 70) * SECSPERYEAR
        + ((timeptr->tm_year - 69) / 4) * SECSPERDAY
        - ((timeptr->tm_year - 1) / 100) * SECSPERDAY
        + ((timeptr->tm_year + 299) / 400) * SECSPERDAY;
}

#undef SECSPERMIN
#undef MINSPERHOUR
#undef HOURSPERDAY
#undef SECSPERHOUR
#undef SECSPERDAY
#undef DAYSPERWEEK
#undef EPOCH_ADJUSTMENT_DAYS
#undef ADJUSTED_EPOCH_YEAR
#undef ADJUSTED_EPOCH_WDAY
#undef DAYS_PER_ERA
#undef DAYS_PER_CENTURY
#undef DAYS_PER_4_YEARS
#undef DAYS_PER_YEAR
#undef DAYS_IN_JANUARY
#undef DAYS_IN_FEBRUARY
#undef YEARS_PER_ERA
#undef YEAR_BASE
#undef EPOCH_YEAR
#undef EPOCH_WDAY
#undef EPOCH_YEARS_SINCE_LEAP
#undef EPOCH_YEARS_SINCE_CENTURY
#undef EPOCH_YEARS_SINCE_LEAP_CENTURY
#undef isleap

