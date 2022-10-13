#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)
#define array_size(x) ((sizeof(x) / sizeof(*x)) / !(sizeof(x) % sizeof(*x)))

//For Visual Studio's sake
typedef signed char int8_t;
typedef signed short int16_t;
typedef signed long int32_t;
typedef signed long long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned long uint32_t;
typedef unsigned long long uint64_t;

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#define printf interface->textLibrary->Write
#define vbl interface->miscLibrary->WaitForVBlank

//-----------------
//ASSPULL REGISTERS
//-----------------
#define MEM_VRAM	0x0E000000
#define MEM_IO		0x0D000000
#define TEXTMAP		((uint16_t*)(MEM_VRAM))
#define BITMAP		((uint8_t*)(MEM_VRAM))
#define MAP1		((uint16_t*)(MEM_VRAM))
#define MAP2		((uint16_t*)(MEM_VRAM + 0x4000))
#define MAP3		((uint16_t*)(MEM_VRAM + 0x8000))
#define MAP4		((uint16_t*)(MEM_VRAM + 0xC000))
#define TILESET		((uint8_t*)(MEM_VRAM + 0x50000))
#define PALETTE		((uint16_t*)(MEM_VRAM + 0x60000))
#define TEXTFONT	((uint8_t*)(MEM_VRAM + 0x60400))
#define OBJECTS_A	((uint16_t*)(MEM_VRAM + 0x64000))
#define OBJECTS_B	((uint32_t*)(MEM_VRAM + 0x64200))

#define REG_INTRMODE	*(int8_t*)(MEM_IO + 0x0000)
#define REG_SCREENMODE	*(int8_t*)(MEM_IO + 0x0001)
#define REG_LINE		*(volatile uint16_t*)(MEM_IO + 0x0002)
#define REG_TICKCOUNT	*(volatile uint32_t*)(MEM_IO + 0x0004)
#define REG_SCREENFADE	*(int8_t*)(MEM_IO + 0x0008)
#define REG_MAPSET		*(uint8_t*)(MEM_IO + 0x0009)
#define REG_MAPBLEND	*(uint8_t*)(MEM_IO + 0x000A)
#define REG_MAPSHIFT	*(uint8_t*)(MEM_IO + 0x000B)
#define REG_SCROLLX1	*(uint16_t*)(MEM_IO + 0x0010)
#define REG_SCROLLY1	*(uint16_t*)(MEM_IO + 0x0012)
#define REG_SCROLLX2	*(uint16_t*)(MEM_IO + 0x0014)
#define REG_SCROLLY2	*(uint16_t*)(MEM_IO + 0x0016)
#define REG_SCROLLX3	*(uint16_t*)(MEM_IO + 0x0018)
#define REG_SCROLLY3	*(uint16_t*)(MEM_IO + 0x001A)
#define REG_SCROLLX4	*(uint16_t*)(MEM_IO + 0x001C)
#define REG_SCROLLY4	*(uint16_t*)(MEM_IO + 0x001E)
#define REG_WINMASK		*(uint16_t*)(MEM_IO + 0x0020)
#define REG_WINLEFT		*(uint16_t*)(MEM_IO + 0x0022)
#define REG_WINRIGHT	*(uint16_t*)(MEM_IO + 0x0024)
#define REG_MIDIOUT		*(volatile uint8_t*)(MEM_IO + 0x0044)
#define REG_OPLOUT		*(volatile int16_t*)(MEM_IO + 0x0048)
#define REG_CARET		*(uint16_t*)(MEM_IO + 0x0054)
#define REG_TIMET		*(int64_t*)(MEM_IO + 0x0060)
#define REG_PCM1OFFSET	*(int32_t*)(MEM_IO + 0x0070)
#define REG_PCM2OFFSET	*(int32_t*)(MEM_IO + 0x0074)
#define REG_PCM1LENGTH	*(int32_t*)(MEM_IO + 0x0078)
#define REG_PCM2LENGTH	*(int32_t*)(MEM_IO + 0x007C)
#define REG_PCM1VOLUMEL	*(int8_t*)(MEM_IO + 0x0080)
#define REG_PCM1VOLUMER	*(int8_t*)(MEM_IO + 0x0081)
#define REG_PCM2VOLUMEL	*(int8_t*)(MEM_IO + 0x0080)
#define REG_PCM2VOLUMER	*(int8_t*)(MEM_IO + 0x0081)
#define REG_DMASOURCE	*(volatile uint32_t*)(MEM_IO + 0x0100)
#define REG_DMATARGET	*(volatile uint32_t*)(MEM_IO + 0x0104)
#define REG_DMALENGTH	*(volatile uint32_t*)(MEM_IO + 0x0108)
#define REG_DMACONTROL	*(volatile uint8_t*)(MEM_IO + 0x010A)
#define REG_HDMACONTROL	((uint32_t*)(MEM_IO + 0x0180))
#define REG_HDMASOURCE	((uint32_t*)(MEM_IO + 0x01A0))
#define REG_HDMATARGET	((uint32_t*)(MEM_IO + 0x01C0))
#define REG_BLITCONTROL	*(uint32_t*)(MEM_IO + 0x0200)
#define REG_BLITSOURCE	*(uint32_t*)(MEM_IO + 0x0204)
#define REG_BLITTARGET	*(uint32_t*)(MEM_IO + 0x0208)
#define REG_BLITLENGTH	*(uint32_t*)(MEM_IO + 0x020C)
#define REG_BLITKEY		*(uint32_t*)(MEM_IO + 0x0210)

#define PCMOFFSET		((int32_t*)(MEM_IO + 0x0070))
#define PCMLENGTH		((int32_t*)(MEM_IO + 0x0078))
#define PCMVOLUME		((int8_t*)(MEM_IO + 0x0080))

#define INP_KEYIN		*(volatile uint8_t*)(0x02000000 + 0x0002)
#define INP_KEYSHIFT	*(volatile uint8_t*)(0x02000000 + 0x0003)
#define INP_JOYSTATES	*(uint8_t*)(0x02000000 + 0x0010)
#define INP_JOYPAD1		*(volatile uint16_t*)(0x02000000 + 0x0012)
#define INP_JOYSTK1H	*(volatile int8_t*)(0x02000000 + 0x0014)
#define INP_JOYSTK1V	*(volatile int8_t*)(0x02000000 + 0x0015)
#define INP_JOYPAD2		*(volatile uint16_t*)(0x02000000 + 0x0016)
#define INP_JOYSTK2H	*(volatile int8_t*)(0x02000000 + 0x0017)
#define INP_JOYSTK2V	*(volatile int8_t*)(0x02000000 + 0x0018)
#define INP_MOUSE		*(uint16_t*)(0x02000000 + 0x0020)
#define INP_KEYMAP		((volatile uint8_t*)(0x02000000 + 0x0040))

#define DMA_BYTE 0
#define DMA_SHORT 1
#define DMA_INT 2
#define DMA_ENABLE 0x01
#define HDMA_DOUBLE 0x80

#define SMODE_TEXT 0		// Specifies PC CGA-style text mode.
#define SMODE_BMP16 1		// Specifies a linear-addressed 16-color four bits per pixel bitmap mode.
#define SMODE_BMP256 2		// Specifies a linear-addressed 256-color eight bits per pixel bitmap mode.
#define SMODE_BOLD 8		// Specifies that text mode should use a bold font.
#define SMODE_TILE 0x63		// Specifies a dual-tilemap system. Forces 320x240 resolution.
#define SMODE_BLINK 0x10	// Specifies that instead of bright background colors, text mode cells should blink.
#define SMODE_240 0x20		// Specifies that instead of 480 lines, the screen height should be halved to 240.
#define SMODE_320 0x40		// Specifies that the screen width should be halved from 640 to 320.
#define SMODE_200 0x80		// Specifies that bitmap modes should be 200 or 400 lines, and text mode 25 or 50 lines.

#define IMODE_DISABLE	0x80	// Enable interrupts.
#define IMODE_INVBLANK	0x04	// VBlank is active.
#define IMODE_INHBLANK	0x02	// HBlank is active.

#define PCM_REPEAT		0x80000000

#define BLIT_COPY				(1 << 0)
#define BLIT_SET				(2 << 0)
#define BLIT_INVERT				(3 << 0)
#define BLIT_UNRLE				(4 << 0)
#define BLIT_STRIDESKIP			(1 << 4)
#define BLIT_COLORKEY			(1 << 5)
#define BLIT_BYTE				(0 << 6)
#define BLIT_SHORT				(1 << 6)
#define BLIT_INT				(2 << 6)
#define BLIT_SOURCESTRIDE(x)	((x) << 8)
#define BLIT_TARGETSTRIDE(x)	((x) << 20)

typedef struct
{
	int32_t Identifier;		// Should always be "AIMG".
	uint8_t BitDepth;		// Should be equal to 4 or 8, for 16 or 256 colors respectively.
	uint8_t Flags;			// Specifies if the image is compressed, among other things.
	uint16_t Width;			// The pixel width of the image.
	uint16_t Height;		// The pixel height of the image.
	uint16_t Stride;		// Specifies how many bytes make up a full line. Should be half the width for a 4bpp image.
	int32_t ByteSize;		// The total amount of bytes making up the full image. Should be equal to stride times height.
	int32_t ColorOffset;	// The offset from the start of the structure to the color data.
	int32_t ImageOffset;	// The offset from the start of the structure to the image data.
	int32_t HdmaOffset;
} TImageFile;

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

typedef enum
{
	LC_CODE,
	LC_DAYS,
	LC_MONS,
	LC_DAYF,
	LC_MONF,
	LC_DATES,
	LC_DATEL,
	LC_TIMES,
	LC_TIMEL,
	LC_CURR,
} ELocale;

typedef struct
{
	unsigned char attribs;
	char numDrives;
	char diskToDev[4];
} TIOState;

typedef struct {
	int8_t	fs_type;		// File system type (0: N/A)
	int8_t	drv;			// Physical drive number
	int8_t	n_fats;			// Number of FATs (1 or 2)
	int8_t	wflag;			// win[] flag (b0: dirty)
	int8_t	fsi_flag;		// FSINFO flags (b7: disabled, b0: dirty)
	int16_t	id;				// File system mount ID
	int16_t	n_rootdir;		// Number of root directory entries (FAT12/16)
	int16_t	csize;			// Cluster size [sectors]
	int32_t	last_clst;		// Last allocated cluster
	int32_t	free_clst;		// Number of free clusters
	int32_t	cdir;			// Current directory start cluster (0: root)
	int32_t	n_fatent;		// Number of FAT entries (number of clusters + 2)
	int32_t	fsize;			// Size of a FAT [sectors]
	int32_t	volbase;		// Volume base sector
	int32_t	fatbase;		// FAT base sector
	int32_t	dirbase;		// Root directory base sector/cluster
	int32_t	database;		// Data base sector
	int32_t	winsect;		// Current sector appearing in the win[]
	int8_t	win[512];		// Disk access window for directory and FAT
} FATFS;

typedef struct {
	FATFS*	fs;				// Pointer to the owner file system object
	int16_t	id;				// Owner file system mount ID
	int8_t	attr;			// Object attribute
	int8_t	stat;			// Object chain status (b1-0: =0:not contiguous, =2:contiguous (no data on FAT), =3:got fragmented, b2:sub-directory stretched)
	int32_t	sclust;			// Object start cluster (0:no cluster or root directory)
	int32_t	objsize;		// Object size (valid when sclust != 0)
} _FDID;

typedef struct {
	_FDID	obj;			// Object identifier (must be the 1st member to detect invalid object pointer)
	int8_t	flag;			// File status flags
	int8_t	err;			// Abort flag (error code)
	int32_t	fptr;			// File read/write pointer (Zeroed on file open)
	int32_t	clust;			// Current cluster of fpter (invalid when fprt is 0)
	int32_t	sect;			// Sector number appearing in buf[] (0:invalid)
	int32_t	dir_sect;		// Sector number containing the directory entry
	int8_t*	dir_ptr;		// Pointer to the directory entry in the win[]
	int8_t	buf[512];		// File private data read/write window
} TFileHandle;

typedef struct {
	_FDID	obj;			// Object identifier
	int32_t	dptr;			// Current read/write offset
	int32_t	clust;			// Current cluster
	int32_t	sect;			// Current sector
	int8_t*	dir;			// Pointer to the directory item in the win[]
	int8_t	fn[12];			// SFN (in/out) {body[8],ext[3],status[1]}
	const int8_t* pat;		// Pointer to the name matching pattern
} TDirHandle;

typedef struct {
	int32_t	fsize;			// File size
	int16_t	fdate;			// Modified date
	int16_t	ftime;			// Modified time
	int8_t	fattrib;		// File attribute
	char	fname[13];		// File name
} TFileInfo;

#define FILE TFileHandle
#define DIR TDirHandle
#define FILEINFO TFileInfo

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2
#define	FA_READ				0x01
#define	FA_WRITE			0x02
#define	FA_OPEN_EXISTING	0x00
#define	FA_CREATE_NEW		0x04
#define	FA_CREATE_ALWAYS	0x08
#define	FA_OPEN_ALWAYS		0x10
#define	FA_OPEN_APPEND		0x30
#define	AM_READONLY			0x01
#define	AM_HIDDEN			0x02
#define	AM_SYSTEM			0x04
#define AM_DIRECTORY		0x10
#define AM_ARCHIVE			0x20

typedef enum {
	FE_NoError, //0. Succeeded
	FE_DiskError, //1. Hard error in IO layer
	FE_IntError, //2. Assert fail
	FE_NoDisk, //3. Drive cannot work -- no disk?
	FE_NoFile, //4. File not found
	FE_NoPath, //5. Path not found
	FE_InvalidPath, //6. Invalid path name format
	FE_Denied, //7. Access denied or directory full
	FE_Denied2, //8. Access denied
	FE_InvalidHnd, //9. Invalid handle
	FE_Protected, //10. Drive is write-protected
	FE_InvalidDrive, //11. Invalid drive number
	FE_NotEnabled, //12. No work area
	FE_NoFS, //13. No FAT file system
	FE_MKFSAbort, //14. Format aborted
	FE_Timeout, //15. Timed out
	FE_Locked, //16. File is in use
	FE_NoLFN, //17. Not enough space for LFN buffer
	FE_TooMany, //18. Too many open files
	FE_InvalidParm, //19. Parameter is invalid
} EFileError;

//--------------
//ASS-RT SUPPORT
//--------------

typedef int64_t time_t;
typedef struct {
	int tm_sec;   //0-59
	int tm_min;   //0-59
	int tm_hour;  //0-23
	int tm_mday;  //1-31
	int tm_mon;   //0-11
	int tm_year;  //
	int tm_wday;  //0-6
	int tm_yday;  //0-356
	int tm_isdst; //
} tm;

extern int strcpy_s(char*, size_t, const char*);
extern int strcat_s(char*, size_t, const char*);
extern size_t strnlen_s(const char*, size_t);
extern int strncmp(const char*, const char*, size_t);
extern char *strchr(const char*, int);
extern char *strrchr(const char*, int);
extern int strkitten_s(char*, size_t, char);
extern int isgraph(int);
extern int isprint(int);
extern int isalpha(int);
extern int isdigit(int);
extern int isalnum(int);
extern int isspace(int);
extern int isupper(int);
extern int ispunct(int);
extern int isblank(int);
extern int isxdigit(int);
extern int iscntrl(int);
extern int isascii(int);
extern int islower(int);
extern int toupper(int);
extern int tolower(int);
extern int toascii(int);
extern char* strcpy(char*, const char*);
extern char* strdup(const char*);
extern int atoi(char*);
extern void* memcpy(void*, const void*, size_t);
extern void* memset(void*, int, size_t);
extern void* malloc(size_t);
extern void* realloc(void*, size_t);
extern void* calloc(size_t, size_t);
extern void free(void*);
extern char* asctime(const tm*);
extern tm* gmtime(const time_t*);
extern time_t mktime(tm*);

//---------
//INTERFACE
//---------

//Text
typedef struct
{
	int(*Write)(const char* format, ...);
	int(*Format)(char* buffer, const char* format, ...);
	void(*WriteChar)(char ch);
	void(*SetCursorPosition)(int left, int top);
	void(*SetTextColor)(int back, int fore);
	void(*SetBold)(bool bold);
	void(*ClearScreen)(void);
} ITextLibrary;

typedef struct
{
	void(*ResetPalette)(void);
	void(*DisplayPicture)(TImageFile* picData);
	void(*Fade)(bool in, bool toWhite);
	void(*DrawString)(const char* str, int x, int y, int color);
	void(*DrawFormat)(const char* format, int x, int y, int color, ...);
	void(*DrawChar)(char ch, int x, int y, int color);
	void(*DrawLine)(int x0, int y0, int x1, int y1, int color, uint8_t* dest);
	void(*FloodFill)(int x, int y, int color, uint8_t* dest);
} IDrawingLibrary;

typedef struct
{
	void(*SetTextMode)(int flags);
	void(*SetBitmapMode16)(int flags);
	void(*SetBitmapMode256)(int flags);
	void(*RemoveObjects)(void);
	void(*WaitForVBlank)(void);
	void(*WaitForVBlanks)(int vbls);
	void(*DmaCopy)(void* dst, const void* src, size_t size, int step);
	void(*DmaClear)(void* dst, int src, size_t size, int step);
	void(*MidiReset)(void);
	void(*RleUnpack)(int8_t* dst, int8_t* src, size_t size);
	char*(*GetLocaleStr)(ELocale category, int item);
} IMiscLibrary;

typedef struct
{
	EFileError(*OpenFile)(TFileHandle* handle, const char* path, char mode);
	EFileError(*CloseFile)(TFileHandle* handle);
	int(*ReadFile)(TFileHandle* handle, void* target, size_t length);
	int(*WriteFile)(TFileHandle* handle, void* source, size_t length);
	uint32_t(*SeekFile)(TFileHandle* handle, uint32_t offset, int origin);
	EFileError(*TruncateFile)(TFileHandle* handle);
	EFileError(*FlushFile)(TFileHandle* handle);
	uint32_t(*FilePosition)(TFileHandle* handle);
	bool(*FileEnd)(TFileHandle* handle);
	size_t(*FileSize)(TFileHandle* handle);
	EFileError(*OpenDir)(TDirHandle* handle, const char* path);
	EFileError(*CloseDir)(TDirHandle* handle);
	EFileError(*ReadDir)(TDirHandle* handle, TFileInfo* info);
	EFileError(*FindFirst)(TDirHandle* handle, TFileInfo* info, const char* path,const char* pattern);
	EFileError(*FindNext)(TDirHandle* handle, TFileInfo* info);
	EFileError(*FileStat)(const char* path, TFileInfo* info);
	EFileError(*UnlinkFile)(const char* path);
	EFileError(*RenameFile)(const char* from, const char* to);
	EFileError(*FileTouch)(const char* path, TFileInfo* dt);
	EFileError(*FileAttrib)(const char* path, char attrib);
	EFileError(*MakeDir)(const char* path);
	EFileError(*ChangeDir)(const char* path);
	EFileError(*GetCurrentDir)(char* buffer, size_t buflen);
	EFileError(*GetLabel)(char disk, char* buffer, uint32_t*);
	const char*(*FileErrStr)(EFileError error);
	uint32_t(*GetFree)(char disk);
} IDiskLibrary;

typedef struct
{
	long AssBang;
	int16_t biosVersion;
	int16_t extensions;
	void(*Exception)(void*);
	void(*VBlank)(void*);
	void(*HBlank)(void*);
	void(*DrawChar)(unsigned char, int, int, int);
	ITextLibrary* textLibrary;
	IDrawingLibrary* drawingLibrary;
	IMiscLibrary* miscLibrary;
	IDiskLibrary* diskLibrary;
	char* DrawCharFont;
	uint16_t DrawCharHeight;
	uint8_t* LinePrinter;
	TIOState io;
	TLocale locale;
} IBios;

#define TEXT interface->textLibrary
#define DRAW interface->drawingLibrary
#define MISC interface->miscLibrary
#define DISK interface->diskLibrary

extern IBios* interface;

//#define DrawChar(g,x,y,c) interface->DrawChar(g,x,y,c)

#ifdef __cplusplus
}
#endif
