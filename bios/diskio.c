#include "diskio.h"
#include "ff.h"

#include <stddef.h>

//Keeping this separate from ass.h because you REALLY have no business here!
#define DEVS ((uint8_t*)0x02000000)
#define DEVSIZE 0x8000
#define DISKRAM &device[0x200]
#define REG_DISKSECTOR *(uint16_t*)(&device[0x002])
#define REG_DISKCONTROL device[0x004]
#define REG_DISKTRACKS device[0x010]
#define REG_DISKHEADS device[0x012]
#define REG_DISKSECTORS device[0x014]
#define DCTL_PRESENT	1
#define DCTL_ERROR		2
#define DCTL_READNOW	4
#define DCTL_WRITENOW	8
#define DCTL_BUSY		16
#define REG_DMASOURCE	*(volatile uint32_t*)(0x0D000100)
#define REG_DMATARGET	*(volatile uint32_t*)(0x0D000104)
#define REG_DMALENGTH	*(volatile uint32_t*)(0x0D000108)
#define REG_DMACONTROL	*(volatile uint8_t*)(0x0D00010A)
#define REG_TIMET		*(volatile int64_t*)(0x0D00060)
typedef struct
{
	unsigned char attribs;
	char numDrives;
	char diskToDev[4];
	char firstDisk[564]; //about the size of a FATFS
} TIOState;
typedef struct
{
	const long assBang;
	const int16_t biosVersion;
	const int16_t extensions;
	void(*exception)(void*);
	void(*vBlank)(void*);
	void(*hBlank)(void*);
	void(*drawChar)(unsigned char, int, int, int);
	const void* textLibrary;
	const void* drawingLibrary;
	const void* miscLibrary;
	const void* diskLibrary;
	char* drawCharFont;
	uint16_t drawCharHeight;
	uint8_t* linePrinter;
	TIOState io;
} IBios;

extern IBios* interface;

extern void* memcpy(void*, const void*, size_t);
extern void* memset(void*, int, size_t);
extern char* strcpy(char* s2, const char* s1);

/*
DSTATUS disk_status(BYTE driveNo) __attribute__ ((weak, alias ("disk_initialize")));
*/

//Define this to make get_fattime NOT choke on the year 2038 by using the full 64-bit time_t.
//FAT16 timestamps range from 1980-01-01 to 2099-12-31, but when you have to get a new one...
//#define YEAR2038
//Using a 32-bit time_t (casting lol) saves about point six KB.

#define SECSPERMIN 60L
#define MINSPERHOUR 60L
#define HOURSPERDAY 24L
#define SECSPERHOUR (SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY (SECSPERHOUR * HOURSPERDAY)
#define EPOCH_ADJUSTMENT_DAYS 719468L
#define ADJUSTED_EPOCH_YEAR 0 //year to which the adjustment was made
#define DAYS_PER_ERA 146097L //there are 97 leap years in 400-year periods. ((400 - 97) * 365 + 97 * 366)
#define DAYS_PER_CENTURY 36524L //there are 24 leap years in 100-year periods. ((100 - 24) * 365 + 24 * 366)
#define DAYS_PER_4_YEARS (3 * 365 + 366) //there is one leap year every 4 years
#define DAYS_PER_YEAR 365 //number of days in a non-leap year
#define YEARS_PER_ERA 400 //number of years per era
DWORD get_fattime(void)
{
#ifdef YEAR2038
	const int64_t lcltime = REG_TIMET;
#else
	const int32_t lcltime = (int32_t)REG_TIMET;
#endif
	if (lcltime == 0)
		return ((DWORD)(FF_NORTC_YEAR - 1980) << 25 | (DWORD)FF_NORTC_MON << 21 | (DWORD)FF_NORTC_MDAY << 16);

	int hours, mins, secs;
	int32_t days, rem;
	int era, year;
	uint32_t erayear, yearday, month, day;
	uint32_t eraday;

	days = lcltime / SECSPERDAY + EPOCH_ADJUSTMENT_DAYS;
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
#undef EPOCH_ADJUSTMENT_DAYS
#undef ADJUSTED_EPOCH_YEAR
#undef DAYS_PER_ERA
#undef DAYS_PER_CENTURY
#undef DAYS_PER_4_YEARS
#undef DAYS_PER_YEAR
#undef YEARS_PER_ERA

DSTATUS disk_initialize(BYTE driveNo)
{
	volatile uint8_t* device = DEVS + (interface->io.diskToDev[driveNo] * DEVSIZE);
	if (REG_DISKCONTROL & DCTL_PRESENT)
		return 0;
	return STA_NOINIT; //STA_NODISK?
}

DSTATUS disk_status(BYTE driveNo)
{
	volatile uint8_t* device = DEVS + (interface->io.diskToDev[driveNo] * DEVSIZE);
	if (REG_DISKCONTROL & DCTL_PRESENT)
		return 0;
	return STA_NOINIT; //STA_NODISK?
}

DRESULT disk_read(BYTE driveNo, BYTE *buff, DWORD sector, UINT count)
{
	volatile uint8_t* device = DEVS + (interface->io.diskToDev[driveNo] * DEVSIZE);
	while(REG_DISKCONTROL & DCTL_BUSY) { ; }
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
	volatile uint8_t* device = DEVS + (interface->io.diskToDev[driveNo] * DEVSIZE);
	while(REG_DISKCONTROL & DCTL_BUSY) { ; }
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
	uint8_t* device = DEVS + (interface->io.diskToDev[driveNo] * DEVSIZE);
	if (!(REG_DISKCONTROL & DCTL_PRESENT))
		return RES_NOTRDY;
	while(REG_DISKCONTROL & DCTL_BUSY) { ; }
	switch (ctrl)
	{
		case CTRL_SYNC: break;
		case GET_SECTOR_COUNT: *(DWORD*)buff = (REG_DISKTRACKS * REG_DISKHEADS) * REG_DISKSECTORS; break;
		case GET_SECTOR_SIZE: *(WORD*)buff = 512; break;
		case GET_BLOCK_SIZE: *(DWORD*)buff = 1; break;
	}
	return RES_OK;
}

#define array_size(x) ((sizeof(x) / sizeof(*x)) / !(sizeof(x) % sizeof(*x)))

//Custom boot sector designed and implemented by Kawa~
/* This is an Asspull IIIx diskette. It won't boot.
 * --------------------------------------------------
 * Remove this disk and press any key to try again.
 */
static const char format_0[] =
{
	0xE9, 0x3B, 0x00, 0x41, 0x53, 0x53, 0x50, 0x55, 0x4C, 0x4C, 0x21, 0x00, 0x02, 0x01, 0x01, 0x00,
	0x02, 0xE0, 0x00, 0x40, 0x0B, 0xF0, 0x09, 0x00, 0x12, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0xB8, 0xC0,
	0x07, 0x8E, 0xD8, 0x8E, 0xC0, 0xB8, 0x00, 0x13, 0xB3, 0x07, 0xB1, 0x0A, 0xBA, 0x01, 0x01, 0xBD,
	0xB6, 0x00, 0xCD, 0x10, 0xB8, 0x02, 0x13, 0xB1, 0x0C, 0xBA, 0x0C, 0x01, 0xBD, 0xC0, 0x00, 0xCD,
	0x10, 0xB8, 0x01, 0x13, 0xB1, 0x1A, 0xBA, 0x19, 0x01, 0xBD, 0xD8, 0x00, 0xCD, 0x10, 0xB8, 0xC4,
	0x09, 0xB3, 0x0F, 0xB1, 0x32, 0xCD, 0x10, 0xB8, 0x00, 0x02, 0xBA, 0x32, 0x02, 0xCD, 0x10, 0xB8,
	0xC4, 0x09, 0xB3, 0x07, 0xB1, 0x01, 0xCD, 0x10, 0xB8, 0x00, 0x02, 0xBA, 0x33, 0x02, 0xCD, 0x10,
	0xB8, 0xC4, 0x09, 0xB3, 0x08, 0xB1, 0x01, 0xCD, 0x10, 0xB8, 0x01, 0x13, 0xB3, 0x07, 0xB1, 0x30,
	0xBA, 0x01, 0x04, 0xBD, 0xF2, 0x00, 0xCD, 0x10, 0x30, 0xE4, 0xCD, 0x16, 0xB4, 0x0F, 0xCD, 0x10,
	0x30, 0xE4, 0xCD, 0x10, 0xCD, 0x19, 0x54, 0x68, 0x69, 0x73, 0x20, 0x69, 0x73, 0x20, 0x61, 0x6E,
	0x41, 0x0B, 0x73, 0x0B, 0x73, 0x0B, 0x70, 0x0B, 0x75, 0x0B, 0x6C, 0x0B, 0x6C, 0x0B, 0x20, 0x07,
	0x49, 0x09, 0x49, 0x09, 0x49, 0x09, 0x78, 0x0C, 0x64, 0x69, 0x73, 0x6B, 0x65, 0x74, 0x74, 0x65,
	0x2E, 0x20, 0x49, 0x74, 0x20, 0x77, 0x6F, 0x6E, 0x27, 0x74, 0x20, 0x62, 0x6F, 0x6F, 0x74, 0x2E,
	0x0D, 0x0A, 0x52, 0x65, 0x6D, 0x6F, 0x76, 0x65, 0x20, 0x74, 0x68, 0x69, 0x73, 0x20, 0x64, 0x69,
	0x73, 0x6B, 0x20, 0x61, 0x6E, 0x64, 0x20, 0x70, 0x72, 0x65, 0x73, 0x73, 0x20, 0x61, 0x6E, 0x79,
	0x20, 0x6B, 0x65, 0x79, 0x20, 0x74, 0x6F, 0x20, 0x74, 0x72, 0x79, 0x20, 0x61, 0x67, 0x61, 0x69,
	0x6E, 0x2E
};

void FormatDisk(int driveNo) //, const char* label)
{
	BYTE buff[512];

	memset(buff, 0, 512);
	memcpy(buff, format_0, array_size(format_0));

	*(int32_t*)(buff + 0x27) = get_fattime();
	//strcpy(buff + 0x2B, label);
	*(int16_t*)(buff + 0x1FE) = 0x55AA;

	//Allow different sized diskettes?

	disk_write(driveNo, buff, 0, 1);

	memset(buff, 0, 512);
	for (int32_t i = 1; i < 256; i++)
		disk_write(driveNo, buff, i, 1);

	//Set up blank chains
	*(int32_t*)buff = 0xF0FFFF00;
	disk_write(driveNo, buff, 1, 1);
	disk_write(driveNo, buff, 10, 1);
}
