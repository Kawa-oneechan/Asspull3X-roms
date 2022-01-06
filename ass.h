#pragma once

#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)

#define DEBUG

#if !defined NULL
#define NULL (void*)0
#endif

#ifndef DEBUG
#define dpf(x, ...)
#else
extern void dpf(const char* format, ...);
#endif

#include "ass-types.h"

#if !defined va_start
typedef int8_t* va_list;
#define _INTSIZEOF(n)	( (sizeof(n) + sizeof(int32_t) - 1) & ~(sizeof(int32_t) - 1) )
#define va_start(ap,v)	( ap = (va_list)&(v) + _INTSIZEOF(v) )
#define va_arg(ap,t)	( *(t *)((ap += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_end(ap)		( ap = (va_list)0 )
#endif

#define printf interface->textLibrary->Write
#define vbl interface->miscLibrary->WaitForVBlank
#define intoff() REG_INTRMODE |= IMODE_DISABLE
#define inton() REG_INTRMODE &= ~IMODE_DISABLE

#define HaveDisk()		(*(volatile int8_t*)(0x02000004) & 1)

//--------------
//ASS-RT SUPPORT
//--------------
extern int8_t* strncpy(char*, const char*, int32_t);
extern int32_t strcpy_s(char*, int32_t, const char*);
extern int32_t strcat_s(char*, int32_t, const char*);
extern int32_t strnlen_s(const char*, int32_t);
extern int32_t strkitten_s(char*, int32_t, char);
extern void* memcpy(void*, const void*, int32_t);
extern void* memset(void*, int32_t, int32_t);
extern void* malloc(uint32_t);
extern void* realloc(void*, int32_t);
extern void free(void*);

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
#define TEXTFONT	((uint8_t*)(MEM_VRAM + 0x60200))
#define OBJECTS_A	((uint16_t*)(MEM_VRAM + 0x64000))
#define OBJECTS_B	((uint32_t*)(MEM_VRAM + 0x64200))

#define REG_INTRMODE	*(int8_t*)(MEM_IO + 0x0000)
#define REG_SCREENMODE	*(int8_t*)(MEM_IO + 0x0001)
#define REG_LINE		*(volatile uint16_t*)(MEM_IO + 0x0002)
#define REG_TICKCOUNT	*(volatile uint32_t*)(MEM_IO + 0x0004)
#define REG_SCREENFADE	*(int8_t*)(MEM_IO + 0x0008)
#define REG_MAPSET		*(uint8_t*)(MEM_IO + 0x0009)
#define REG_MAPBLEND	*(uint8_t*)(MEM_IO + 0x000A)
#define REG_SCROLLX1	*(uint16_t*)(MEM_IO + 0x0010)
#define REG_SCROLLY1	*(uint16_t*)(MEM_IO + 0x0012)
#define REG_SCROLLX2	*(uint16_t*)(MEM_IO + 0x0014)
#define REG_SCROLLY2	*(uint16_t*)(MEM_IO + 0x0016)
#define REG_SCROLLX3	*(uint16_t*)(MEM_IO + 0x0018)
#define REG_SCROLLY3	*(uint16_t*)(MEM_IO + 0x001A)
#define REG_SCROLLX4	*(uint16_t*)(MEM_IO + 0x001C)
#define REG_SCROLLY4	*(uint16_t*)(MEM_IO + 0x001E)
#define REG_KEYIN		*(volatile uint16_t*)(MEM_IO + 0x0040)
#define REG_JOYPAD1		*(volatile uint8_t*)(MEM_IO + 0x0042)
#define REG_JOYPAD2		*(volatile uint8_t*)(MEM_IO + 0x0043)
#define REG_MIDIOUT		*(int8_t*)(MEM_IO + 0x0044)
#define REG_OPLOUT		*(int16_t*)(MEM_IO + 0x0048)
#define REG_MOUSE		*(uint16_t*)(MEM_IO + 0x0050)
#define REG_CARET		*(uint16_t*)(MEM_IO + 0x0054)
#define REG_PCMOFFSET	*(int32_t*)(MEM_IO + 0x0070)
#define REG_PCMLENGTH	*(int32_t*)(MEM_IO + 0x0074)
#define REG_DEBUGOUT	*(char*)(MEM_IO + 0x0080)
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

#define JOYPADS		((volatile uint8_t*)(MEM_IO + 0x0042))
#define REG_JOYPAD	REG_JOYPAD1

#define DMA_BYTE 0
#define DMA_SHORT 1
#define DMA_INT 2
#define DMA_ENABLE 0x01
#define HDMA_DOUBLE 0x80

#define SMODE_TEXT 0		// Specifies PC CGA-style text mode.
#define SMODE_BMP16 1		// Specifies a linear-addressed 16-color four bits per pixel bitmap mode.
#define SMODE_BMP256 2		// Specifies a linear-addressed 256-color eight bits per pixel bitmap mode.
#define SMODE_TILE 0x63		// Specifies a dual-tilemap system. Forces 320x240 resolution.
#define SMODE_BLINK 0x10	// Specifies that instead of bright background colors, text mode cells should blink.
#define SMODE_240 0x20		// Specifies that instead of 480 lines, the screen height should be halved to 240.
#define SMODE_320 0x40		// Specifies that the screen width should be halved from 640 to 320.
#define SMODE_BOLD 0x80		// Specifies that text mode should use a bold font.
#define SMODE_200 0x80		// Specifies that bitmap modes should be 200 or 400 lines.

#define IMODE_DISABLE	0x80	// Enable interrupts.
#define IMODE_INVBLANK	0x04	// VBlank is triggered.

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

typedef struct TImageFile
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
} TImageFile;

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

typedef struct TFileHandle {
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

typedef struct TDirHandle {
	_FDID	obj;			// Object identifier
	int32_t	dptr;			// Current read/write offset
	int32_t	clust;			// Current cluster
	int32_t	sect;			// Current sector
	int8_t*	dir;			// Pointer to the directory item in the win[]
	int8_t	fn[12];			// SFN (in/out) {body[8],ext[3],status[1]}
	const int8_t* pat;		// Pointer to the name matching pattern
} TDirHandle;

typedef struct TFileInfo {
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

//Text
typedef struct ITextLibrary
{
	int(*Write)(const char* format, ...);
	int(*Format)(char* buffer, const char* format, ...);
	void(*WriteChar)(char ch);
	void(*SetBold)(int32_t bold);
	void(*SetCursorPosition)(int32_t left, int32_t top);
	void(*SetTextColor)(int32_t back, int32_t fore);
	void(*ClearScreen)(void);
} ITextLibrary;

typedef struct IDrawingLibrary
{
	void(*ResetPalette)(void);
	void(*DisplayPicture)(TImageFile* picData);
	void(*FadeToBlack)(void);
	void(*FadeFromBlack)(void);
	void(*FadeToWhite)(void);
	void(*FadeFromWhite)(void);
	void(*DrawString)(const char* str, int32_t x, int32_t y, int32_t color);
	void(*DrawFormat)(const char* format, int32_t x, int32_t y, int32_t color, ...);
	void(*DrawChar)(char ch, int32_t x, int32_t y, int32_t color);
	void(*DrawLine)(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color, uint8_t* dest);
	void(*FloodFill)(int32_t x, int32_t y, int32_t color);
} IDrawingLibrary;

typedef struct IMiscLibrary
{
	void(*SetTextMode)(int32_t flags);
	void(*SetBitmapMode16)(int32_t flags);
	void(*SetBitmapMode256)(int32_t flags);
	void(*EnableObjects)(int32_t);
	void(*WaitForVBlank)(void);
	void(*WaitForVBlanks)(int32_t vbls);
	void(*DmaCopy)(void* dst, const void* src, uint32_t size, int32_t step);
	void(*DmaClear)(void* dst, int32_t src, uint32_t size, int32_t step);
	void(*MidiReset)(void);
	void(*RleUnpack)(int8_t* dst, int8_t* src, uint32_t size);
} IMiscLibrary;

typedef struct IDiskLibrary
{
	int32_t(*OpenFile)(TFileHandle* handle, const char* path, char mode);
	int32_t(*CloseFile)(TFileHandle* handle);
	int32_t(*ReadFile)(TFileHandle* handle, void* target, uint32_t length);
	int32_t(*WriteFile)(TFileHandle* handle, void* source, uint32_t length);
	int32_t(*SeekFile)(TFileHandle* handle, uint32_t offset, int32_t origin);
	int32_t(*TruncateFile)(TFileHandle* handle);
	int32_t(*FlushFile)(TFileHandle* handle);
	uint32_t(*FilePosition)(TFileHandle* handle);
	int32_t(*FileEnd)(TFileHandle* handle);
	uint32_t(*FileSize)(TFileHandle* handle);
	int32_t(*OpenDir)(TDirHandle* handle, const char* path);
	int32_t(*CloseDir)(TDirHandle* handle);
	int32_t(*ReadDir)(TDirHandle* handle, TFileInfo* info);
	int32_t(*FindFirst)(TDirHandle* handle, TFileInfo* info, const char* path,const char* pattern);
	int32_t(*FindNext)(TDirHandle* handle, TFileInfo* info);
	int32_t(*FileStat)(const char* path, TFileInfo* info);
	int32_t(*UnlinkFile)(const char* path);
	int32_t(*RenameFile)(const char* from, const char* to);
	int32_t(*FileTouch)(const char* path, TFileInfo* dt);
	int32_t(*MakeDir)(const char* path);
	int32_t(*ChangeDir)(const char* path);
	int32_t(*GetCurrentDir)(char* buffer, int32_t buflen);
	int32_t(*GetLabel)(char* buffer);
	const char*(*FileErrStr)(int32_t error);
	int32_t(*GetNumDrives)(void);
} IDiskLibrary;

typedef struct IBios
{
	long AssBang;
	int16_t biosVersion;
	int16_t extensions;
	void(*Exception)(void);
	void(*VBlank)(void);
	void(*reserved)(void);
	void(*DrawChar)(char, int32_t, int32_t, int32_t);
	ITextLibrary* textLibrary;
	IDrawingLibrary* drawingLibrary;
	IMiscLibrary* miscLibrary;
	IDiskLibrary* diskLibrary;
	char* DrawCharFont;
	uint16_t DrawCharHeight;
} IBios;

#define TEXT interface->textLibrary
#define DRAW interface->drawingLibrary
#define MISC interface->miscLibrary
#define DISK interface->diskLibrary

extern IBios* interface;

//#define DrawChar(g,x,y,c) interface->DrawChar(g,x,y,c)
