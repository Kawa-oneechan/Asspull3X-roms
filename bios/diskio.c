#include "diskio.h"
#include "ff.h"

//TODO: allow multiple drives

//Keeping this separate from ass.h because you REALLY have no business here!
#define DISKRAM			((unsigned char*)0x02000200)
#define REG_DISKSECTOR	*(unsigned short*)(0x02000002)
#define REG_DISKCONTROL	*(volatile unsigned char*)(0x02000004)
#define REG_DISKTRACKS	*(unsigned short*)(0x02000010)
#define REG_DISKHEADS	*(unsigned short*)(0x02000012)
#define REG_DISKSECTORS	*(unsigned short*)(0x02000014)
#define DCTL_PRESENT	1
#define DCTL_ERROR		2
#define DCTL_READNOW	4
#define DCTL_WRITENOW	8
#define REG_DMASOURCE	*(volatile unsigned long*)(0x0D000100)
#define REG_DMATARGET	*(volatile unsigned long*)(0x0D000104)
#define REG_DMALENGTH	*(volatile unsigned long*)(0x0D000108)
#define REG_DMACONTROL	*(volatile unsigned char*)(0x0D00010A)
#define REG_TIMET		*(volatile long long*)(0x0D00060)

/*
DSTATUS disk_initialize(BYTE driveNo)
{
	if (REG_DISKCONTROL & DCTL_PRESENT)
		return 0;
	return STA_NOINIT; //STA_NODISK?
}
DSTATUS disk_status(BYTE driveNo) __attribute__ ((weak, alias ("disk_initialize")));
*/
/*
DSTATUS disk_status(BYTE driveNo)
{
	if (REG_DISKCONTROL & DCTL_PRESENT)
		return 0;
	return STA_NOINIT; //STA_NODISK?
}
*/

#define SECSPERMIN 60L
#define MINSPERHOUR 60L
#define HOURSPERDAY 24L
#define SECSPERHOUR (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY (SECSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK 7
#define MONSPERYEAR 12
#define EPOCH_ADJUSTMENT_DAYS 719468L
#define ADJUSTED_EPOCH_YEAR 0 //year to which the adjustment was made
#define ADJUSTED_EPOCH_WDAY 3 //1st March of year 0 is Wednesday
#define DAYS_PER_ERA 146097L //there are 97 leap years in 400-year periods. ((400 - 97) * 365 + 97 * 366)
#define DAYS_PER_CENTURY 36524L //there are 24 leap years in 100-year periods. ((100 - 24) * 365 + 24 * 366)
#define DAYS_PER_4_YEARS (3 * 365 + 366) //there is one leap year every 4 years
#define DAYS_PER_YEAR 365 //number of days in a non-leap year
#define DAYS_IN_JANUARY 31 //number of days in January
#define DAYS_IN_FEBRUARY 28 //number of days in non-leap February
#define YEARS_PER_ERA 400 //number of years per era
#define YEAR_BASE 1900
#define EPOCH_YEAR 1970
#define EPOCH_WDAY 4
#define EPOCH_YEARS_SINCE_LEAP 2
#define EPOCH_YEARS_SINCE_CENTURY 70
#define EPOCH_YEARS_SINCE_LEAP_CENTURY 370
#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)
DWORD get_fattime(void)
{
	const long long lcltime = REG_TIMET;
	if (lcltime == 0)
		return ((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16);

	int hours, mins, secs;
	long days, rem;
	int era, year;
	unsigned erayear, yearday, month, day;
	unsigned long eraday;

	days = lcltime / ((SECSPERMIN * MINSPERHOUR) * HOURSPERDAY) + EPOCH_ADJUSTMENT_DAYS;
	rem = lcltime % SECSPERDAY;
	if (rem < 0)
	{
		rem += SECSPERDAY;
		--days;
	}

	hours = (int) (rem / SECSPERHOUR);
	rem %= SECSPERHOUR;
	mins = (int) (rem / SECSPERMIN);
	secs = (int) (rem % SECSPERMIN);

	era = (days >= 0 ? days : days - (DAYS_PER_ERA - 1)) / DAYS_PER_ERA;
	eraday = days - era * DAYS_PER_ERA;	//[0, 146096]
	erayear = (eraday - eraday / (DAYS_PER_4_YEARS - 1) + eraday / DAYS_PER_CENTURY -
	eraday / (DAYS_PER_ERA - 1)) / 365;	//[0, 399]
	yearday = eraday - (DAYS_PER_YEAR * erayear + erayear / 4 - erayear / 100);	//[0, 365]
	month = (5 * yearday + 2) / 153;	//[0, 11]
	day = yearday - (153 * month + 2) / 5 + 1;	//[1, 31]
	month += month < 10 ? 2 : -10;
	year = ADJUSTED_EPOCH_YEAR + erayear + era * YEARS_PER_ERA + (month <= 1);

	return ((DWORD)(year - 1980) << 25 | (DWORD)month << 21 | (DWORD)day << 16) | (DWORD)hours << 11 | (DWORD)mins << 5 | (DWORD)secs;
}
#undef SECSPERMIN
#undef MINSPERHOUR
#undef HOURSPERDAY
#undef SECSPERHOUR
#undef SECSPERDAY
#undef DAYSPERWEEK
#undef MONSPERYEAR
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

DRESULT disk_read(BYTE driveNo, BYTE *buff, DWORD sector, UINT count)
{
	while(count--)
	{
		REG_DISKSECTOR = sector++;
		REG_DISKCONTROL = DCTL_READNOW;
		if (REG_DISKCONTROL & DCTL_ERROR)
			return RES_PARERR;
		REG_DMASOURCE = (int)DISKRAM;
		REG_DMATARGET = (int)buff;
		REG_DMALENGTH = 512;
		REG_DMACONTROL = 0x07;
		buff += 512;
	}
	return 0;
}

DRESULT disk_write(BYTE driveNo, const BYTE *buff, DWORD sector, UINT count)
{
	while(count--)
	{
		REG_DISKSECTOR = sector++;
		REG_DMASOURCE = (int)buff;
		REG_DMATARGET = (int)DISKRAM;
		REG_DMALENGTH = 512;
		REG_DMACONTROL = 0x07;
		REG_DISKCONTROL = DCTL_WRITENOW;
		buff += 512;
	}
	return 0;
}

DRESULT disk_ioctl(BYTE driveNo, BYTE ctrl, void *buff)
{
	if (!(REG_DISKCONTROL & DCTL_PRESENT))
		return RES_NOTRDY;
	switch (ctrl)
	{
		case CTRL_SYNC: break;
		case GET_SECTOR_COUNT: *(DWORD*)buff = (REG_DISKTRACKS * REG_DISKHEADS) * REG_DISKSECTORS; break;
		case GET_SECTOR_SIZE: *(WORD*)buff = 512; break;
		case GET_BLOCK_SIZE: *(DWORD*)buff = 1; break;
	}
	return RES_OK;
}
