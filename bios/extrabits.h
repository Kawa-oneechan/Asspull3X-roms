//Just the bits of asspull.h that we need for printf and diskio.

#include <stdbool.h>

typedef struct
{
	unsigned char attribs;
	char numDrives;
	char diskToDev[4];
	char firstDisk[564]; //about the size of a FATFS
} TIOState;

typedef struct
{
	char code[6];			//Locale name
	char wday_name[32];		//"Sun"..."Sat"
	char mon_name[64];		//"Jan"..."Dec"
	char wday_nameF[64];	//"Sunday"..."Saturday"
	char mon_nameF[106];	//"January"..."December"
	char shortDateFmt[16];	//Format for "1983-06-26"
	char longDateFmt[16];	//Format for "Sunday, June 26, 1983"
	char shortTimeFmt[16];	//Format for "17:42:07"
	char longTimeFmt[16];	//Format for "17:42:07"
	char thousands;			//What to put between clusters of three digits, ','
	char decimals;			//What to put between an integer and decimals, '.'
	char thousandsCt;		//How many digits per cluster, 3
	char currency[4];		//Currency symbol, '$'
	bool currencyAfter;		//Is it "10$" or "$10"?
	char reserved[16];
	char sctoasc[256];		//Scancode map
} TLocale;

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
	TLocale locale;
} IBios;

extern IBios* interface;
