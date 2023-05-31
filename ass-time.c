#include "ass.h"
extern IBios* interface;

//----
//TIME
//----

#define YEAR_BASE 1900

char* asctime_r(const tm *timeptr, char* buf)
{
	TEXT->Format(buf, "%.3s %.3s%3d %.2d:%.2d:%.2d %d",
		MISC->GetLocaleStr(LC_DAYS, timeptr->tm_wday),
		MISC->GetLocaleStr(LC_MONS, timeptr->tm_mon),
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

char* ctime_r(const time_t* timer, char* buf)
{
	return asctime_r(gmtime(timer), buf);
}

char* ctime(const time_t* timer)
{
	return asctime(gmtime(timer));
}
