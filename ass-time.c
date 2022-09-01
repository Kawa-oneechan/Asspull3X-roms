#include "ass.h"
extern IBios* interface;

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

static const char wday_name[][4] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};
static const char mon_name[][4] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
static const char wday_nameF[][10] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
static const char mon_nameF[][10] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
};

size_t strftime(char *str, size_t count, const char *format, const tm *time)
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
						len = TEXT->Format(s, "%d", time->tm_year + YEAR_BASE);
						goto next;
					case 'y':
						len = TEXT->Format(s, "%d", time->tm_year + YEAR_BASE);
						len -= 2;
						s[0] = s[2];
						s[1] = s[3];
						goto next;
					case 'C':
						len = TEXT->Format(s, "%d", time->tm_year + YEAR_BASE);
						len -= 2;
						goto next;
					case 'b':
					case 'h':
						len = TEXT->Format(s, "%s", mon_name[time->tm_mon]);
						goto next;
					case 'B':
						len = TEXT->Format(s, "%s", mon_nameF[time->tm_mon]);
						goto next;
					case 'm':
						len = TEXT->Format(s, "%02d", time->tm_mon + 1);
						goto next;
					//case 'U':
					//case 'W':
					//case 'V':
					case 'j':
						len = TEXT->Format(s, "%.3d", time->tm_yday);
						goto next;
					case 'd':
						len = TEXT->Format(s, "%02d", time->tm_mday);
						goto next;
					case 'e':
						len = TEXT->Format(s, "%.2d", time->tm_mday);
						goto next;
					case 'a':
						len = TEXT->Format(s, "%s", wday_name[time->tm_wday]);
						goto next;
					case 'A':
						len = TEXT->Format(s, "%s", wday_nameF[time->tm_wday]);
						goto next;
					case 'w':
						len = TEXT->Format(s, "%d", time->tm_wday);
						goto next;
					case 'H':
						len = TEXT->Format(s, "%02d", time->tm_hour);
						goto next;
					//case 'I':
					case 'M':
						len = TEXT->Format(s, "%02d", time->tm_min);
						goto next;
					case 'S':
						len = TEXT->Format(s, "%02d", time->tm_sec);
						goto next;
					case 'c':
						len = strftime(s, count, "%a %h %d %H:%M:%S %Y", time);
						goto next;
					case 'D': //never do this
						len = strftime(s, count, "%m/%d/%y", time);
						goto next;
					case 'F': //always do this
						len = strftime(s, count, "%Y-%m-%d", time);
						goto next;
					case 'R':
						len = strftime(s, count, "%H:%M", time);
						goto next;
					case 'T':
						len = strftime(s, count, "%H:%M:%S", time);
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

char* asctime_r(const tm *timeptr, char* buf)
{
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
	int16_t days, rem;
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

