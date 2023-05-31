#include "../ass.h"
#include "funcs.h"

extern int vsprintf(char*, const char*, va_list);

const IMiscLibrary miscLibrary =
{
	WaitForVBlank, WaitForVBlanks,
	DmaCopy, DmaClear,
	MidiReset, OplReset,
	RleUnpack,
	GetLocaleStr,
	HeapAlloc, HeapFree, HeapReAlloc, HeapCAlloc,
	FormatTime, TimeToCalendar, CalendarToTime,
};

void WaitForVBlank()
{
	uint32_t tickCount = REG_TICKCOUNT;
	while (REG_TICKCOUNT == tickCount);
//	while(REG_LINE >= 480);
//	while(REG_LINE < 480);
}

void WaitForVBlanks(int vbls)
{
	while(vbls--) WaitForVBlank();
}

//CONSIDER: Replace these with #define macros in ass.h.
void DmaCopy(void* dst, const void* src, size_t size, int step)
{
	REG_DMASOURCE = (int)src;
	REG_DMATARGET = (int)dst;
	REG_DMALENGTH = size;
	REG_DMACONTROL = 0x07 | (step << 4);
}
void DmaClear(void* dst, int src, size_t size, int step)
{
	REG_DMASOURCE = (int)src;
	REG_DMATARGET = (int)dst;
	REG_DMALENGTH = size;
	REG_DMACONTROL = 0x0D | (step << 4);
}

void MidiReset()
{
	for (int i = 0; i < 16; i++)
	{
		REG_MIDIOUT = (0xB0 | i) | 121 << 8; //Reset controllers
		REG_MIDIOUT = (0xB0 | i) | 123 << 8; //All notes off
	}
}

void OplReset()
{
	static const char regs[] = {
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0xA0, 0xA1,
		0xA2, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
		0xB0, 0xB1, 0xB2, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,
		0xB7, 0xB8, 0xBD, 0xC0, 0xC1, 0xC2, 0xC2, 0xC3,
		0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xE0, 0xE1, 0xE2,
		0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
		0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2,
		0xF3, 0xF4, 0xF5, 0x08, 0x01,
	};
	for (unsigned int i = 0; i < array_size(regs); i++)
		REG_OPLOUT = regs[i] << 8;
}

void RleUnpack(int8_t* dst, int8_t* src, size_t size)
{
	uint8_t data = 0;
	while (size)
	{
		data = *src++;
		if ((data & 0xC0) == 0xC0)
		{
			int8_t len = data & 0x3F;
			data = *src++;
			size--;
			if (data == 0xC0 && len == 0)
				break;
			for (; len > 0; len--)
				*dst++ = data;
		}
		else
		{
			*dst++ = data;
		}
		size--;
	}
}

char* GetLocaleStr(ELocale category, int item)
{
	char* source;
	switch (category)
	{
		case LC_CODE: return interface->locale.code;
		case LC_DAYS: source = interface->locale.wday_name; break;
		case LC_MONS: source = interface->locale.mon_name; break;
		case LC_DAYF: source = interface->locale.wday_nameF; break;
		case LC_MONF: source = interface->locale.mon_nameF; break;
		case LC_DATES: return interface->locale.shortDateFmt;
		case LC_DATEL: return interface->locale.longDateFmt;
		case LC_TIMES: return interface->locale.shortTimeFmt;
		case LC_TIMEL: return interface->locale.longTimeFmt;
		case LC_CURR: return interface->locale.currency;
		default: return "<?>";
	}
	for (; item; item--, source++) for (; *source; source++);
	return source;
}

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
	bool is_free;
} malloc_block_meta;

#define MALLOC_META_SIZE sizeof(struct malloc_block_meta)

void* malloc_global_base = NULL;

static malloc_block_meta* malloc_find_free_block(struct malloc_block_meta** last, size_t size)
{
	malloc_block_meta* current = malloc_global_base;
	while (current && !(current->is_free && current->size >= size))
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
	block->is_free = false;
	return block;
}

void* HeapAlloc(size_t size)
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
			block->is_free = false;
		}
	}

	return (block+1);
}

static malloc_block_meta* malloc_get_block_ptr(void* ptr)
{
	return (malloc_block_meta*)ptr - 1;
}

void HeapFree(void* ptr)
{
	if (!ptr) return;
	//TODO: consider merging blocks once splitting blocks is implemented.
	malloc_block_meta* block_ptr = malloc_get_block_ptr(ptr);
	//assert(block_ptr->free == 0);
	block_ptr->is_free = true;
}

void* HeapReAlloc(void* ptr, size_t size)
{
	if (!ptr) //NULL ptr. realloc should act like malloc.
		return HeapAlloc(size);

	malloc_block_meta* block_ptr = malloc_get_block_ptr(ptr);
	if (block_ptr->size >= size) //We have enough space. Could free some once we implement split.
		return ptr;

	// Need to really realloc. Malloc new space and free old space.
	// Then copy old data to new space.
	void* new_ptr;
	new_ptr = HeapAlloc(size);
	if (!new_ptr) return NULL; //TODO: set errno on failure.

	memcpy(new_ptr, ptr, block_ptr->size);
	HeapFree(ptr);
	return new_ptr;
}

void* HeapCAlloc(size_t nelem, size_t elsize)
{
	size_t size = nelem * elsize; //TODO: check for overflow.
	void* ptr = HeapAlloc(size);
	memset(ptr, 0, size);
	return ptr;
}

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

size_t FormatTime(char *str, size_t count, const char *format, const tm *time)
{
	const char *f = format;
	char *s = str;
	int len = 0;

	while(*f && count)
	{
		if (*f == '%')
		{
			f++;
			if (*f == '%')
			{
				*s++ = '%';
				*s = 0;
				count--;
			}
			else
			{
				switch (*f)
				{
					case 'n':
						*s++ = '\n';
						count--;
						break;
					case 't':
						*s++ = '\t';
						count--;
						break;
					case 'Y':
						len = Format(s, "%d", time->tm_year + YEAR_BASE);
						goto next;
					case 'y':
						len = Format(s, "%d", time->tm_year + YEAR_BASE);
						len -= 2;
						s[0] = s[2];
						s[1] = s[3];
						goto next;
					case 'C':
						len = Format(s, "%d", time->tm_year + YEAR_BASE);
						len -= 2;
						goto next;
					case 'b':
					case 'h':
						len = Format(s, "%s", MISC->GetLocaleStr(LC_MONS, time->tm_mon));
						goto next;
					case 'B':
						len = Format(s, "%s", MISC->GetLocaleStr(LC_MONF, time->tm_mon));
						goto next;
					case 'm':
						len = Format(s, "%02d", time->tm_mon + 1);
						goto next;
					//case 'U':
					//case 'W':
					//case 'V':
					case 'j':
						len = Format(s, "%.3d", time->tm_yday);
						goto next;
					case 'd':
						len = Format(s, "%02d", time->tm_mday);
						goto next;
					case 'e':
						len = Format(s, "%.2d", time->tm_mday);
						goto next;
					case 'a':
						len = Format(s, "%s", MISC->GetLocaleStr(LC_DAYS, time->tm_wday));
						goto next;
					case 'A':
						len = Format(s, "%s", MISC->GetLocaleStr(LC_DAYF, time->tm_wday));
						goto next;
					case 'w':
						len = Format(s, "%d", time->tm_wday);
						goto next;
					case 'H':
						len = Format(s, "%02d", time->tm_hour);
						goto next;
					//case 'I':
					case 'M':
						len = Format(s, "%02d", time->tm_min);
						goto next;
					case 'S':
						len = Format(s, "%02d", time->tm_sec);
						goto next;
					case 'c':
						len = FormatTime(s, count, "%a %h %d %H:%M:%S %Y", time);
						goto next;
					case 'D': //never do this
						len = FormatTime(s, count, "%m/%d/%y", time);
						goto next;
					case 'F': //always do this
						len = FormatTime(s, count, "%Y-%m-%d", time);
						goto next;
					case 'R':
						len = FormatTime(s, count, "%H:%M", time);
						goto next;
					case 'T':
						len = FormatTime(s, count, "%H:%M:%S", time);
						goto next;
					//case 'p':
					}
			next:
				s += len;
				*s = 0;
				count -= len;
			}

			f++;
		}
		else
		{
			*s++ = *f++;
			*s = 0;
			count--;
		}
	}
	return s - str;
}

tm* TimeToCalendar(const time_t* timer)
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

time_t CalendarToTime(tm* timeptr)
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
