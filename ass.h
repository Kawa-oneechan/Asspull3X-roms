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
#define MEM_IO		0x0D800000
#define MAP1		((uint16_t*)(MEM_VRAM))
#define MAP2		((uint16_t*)(MEM_VRAM + 0x4000))
#define MAP3		((uint16_t*)(MEM_VRAM + 0x8000))
#define MAP4		((uint16_t*)(MEM_VRAM + 0xC000))
#define TILESET		((uint8_t*)(MEM_VRAM + 0x10000))
#define PALETTE		((uint16_t*)(MEM_VRAM + 0x50000))
#define SPRITES_A	((uint16_t*)(MEM_VRAM + 0x54000))
#define SPRITES_B	((uint32_t*)(MEM_VRAM + 0x54200))

#define REG_LINE		*(volatile uint16_t*)(MEM_IO + 0x0000)
#define REG_SCREENMODE	*(int8_t*)(MEM_IO + 0x0003)
#define REG_SCREENFADE	*(int8_t*)(MEM_IO + 0x0004)
#define REG_INTRMODE	*(int8_t*)(MEM_IO + 0x0005)
#define REG_KEYIN		*(volatile uint16_t*)(MEM_IO + 0x0006)
#define REG_TICKCOUNT	*(volatile uint32_t*)(MEM_IO + 0x0008)
#define REG_JOYPAD1		*(volatile uint8_t*)(MEM_IO + 0x000C)
#define REG_JOYPAD2		*(volatile uint8_t*)(MEM_IO + 0x000D)
#define REG_DEBUGOUT	*(char*)(MEM_IO + 0x000E)
#define REG_MAPSET		*(uint8_t*)(MEM_IO + 0x000F)
#define REG_SCROLLX1	*(uint16_t*)(MEM_IO + 0x0010)
#define REG_SCROLLY1	*(uint16_t*)(MEM_IO + 0x0012)
#define REG_SCROLLX2	*(uint16_t*)(MEM_IO + 0x0014)
#define REG_SCROLLY2	*(uint16_t*)(MEM_IO + 0x0016)
#define REG_SCROLLX3	*(uint16_t*)(MEM_IO + 0x0018)
#define REG_SCROLLY3	*(uint16_t*)(MEM_IO + 0x001A)
#define REG_SCROLLX4	*(uint16_t*)(MEM_IO + 0x001C)
#define REG_SCROLLY4	*(uint16_t*)(MEM_IO + 0x001E)
#define REG_DMASOURCE	*(volatile uint32_t*)(MEM_IO + 0x0020)
#define REG_DMATARGET	*(volatile uint32_t*)(MEM_IO + 0x0024)
#define REG_DMALENGTH	*(volatile uint32_t*)(MEM_IO + 0x0028)
#define REG_DMACONTROL	*(volatile uint8_t*)(MEM_IO + 0x002A)
#define REG_MIDIOUT		*(volatile int32_t*)(MEM_IO + 0x0040)
#define REG_AUDIOOUT	*(int8_t*)(MEM_IO + 0x0044)
#define REG_HDMACONTROL	((uint32_t*)(MEM_IO + 0x0080))
#define REG_HDMASOURCE	((uint32_t*)(MEM_IO + 0x00A0))
#define REG_HDMATARGET	((uint32_t*)(MEM_IO + 0x00C0))
#define REG_BLITCONTROL	*(uint32_t*)(MEM_IO + 0x0100)
#define REG_BLITSOURCE	*(uint32_t*)(MEM_IO + 0x0104)
#define REG_BLITTARGET	*(uint32_t*)(MEM_IO + 0x0108)
#define REG_BLITLENGTH	*(uint32_t*)(MEM_IO + 0x010C)
#define REG_BLITKEY		*(uint32_t*)(MEM_IO + 0x0110)

#define JOYPADS		((volatile uint8_t*)(MEM_IO + 0x000C))
#define REG_JOYPAD	REG_JOYPAD1

#define DMA_BYTE 0
#define DMA_SHORT 1
#define DMA_INT 2
#define DMA_ENABLE 0x01
#define HDMA_DOUBLE 0x80

#define SMODE_TEXT 0		// Specifies PC CGA-style text mode.
#define SMODE_BMP1 1		// Specifies a linear-addressed 16-color four bits per pixel bitmap mode.
#define SMODE_BMP2 2		// Specifies a linear-addressed 256-color eight bits per pixel bitmap mode.
#define SMODE_TILE 0x63		// Specifies a dual-tilemap system. Forces 320x240 resolution.
#define SMODE_240 0x20		// Specifies that instead of 480 lines, the screen height should be halved to 240.
#define SMODE_320 0x40		// Specifies that the screen width should be halved from 640 to 320.
#define SMODE_BOLD 0x80		// Specifies that text mode should use a bold font.

#define IMODE_DISABLE	0x80	// Enable interrupts.
#define IMODE_INVBLANK	0x04	// VBlank is triggered.
#define IMODE_INHBLANK	0x02	// HBlank is triggered.

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
	int(*Write)(const char*,...);
	int(*Format)(char*,const char*,...);
//	void(*WriteString)(const int8_t*);
//	void(*WriteInt)(int32_t);
	void(*WriteChar)(char);
//	void(*WriteHex)(int32_t);
//	void(*WriteHex8)(int8_t);
//	void(*WriteHex16)(int16_t);
//	void(*WriteHex32)(int32_t);
	void(*SetBold)(int32_t);
	void(*SetCursorPosition)(int32_t, int32_t);
	void(*SetTextColor)(int32_t, int32_t);
	void(*ClearScreen)(void);
} ITextLibrary;

typedef struct IDrawingLibrary
{
	void(*ResetPalette)(void);
	void(*DisplayPicture)(TImageFile*);
	void(*FadeToBlack)(void);
	void(*FadeFromBlack)(void);
	void(*FadeToWhite)(void);
	void(*FadeFromWhite)(void);
	void(*DrawString)(const char*, int32_t, int32_t, int32_t);
	void(*DrawFormat)(const char*, int32_t, int32_t, int32_t, ...);
	void(*DrawChar)(char, int32_t, int32_t, int32_t);
} IDrawingLibrary;

typedef struct IMiscLibrary
{
//	void(*SetTextMode80x30)(void);
//	void(*SetTextMode80x60)(void);
	void(*SetTextMode)(int32_t);
	void(*SetBitmapMode16)(int32_t);
	void(*SetBitmapMode256)(int32_t);
	void(*EnableSprites)(int32_t);
	void(*WaitForVBlank)(void);
	void(*WaitForVBlanks)(int32_t);
	void(*DmaCopy)(void*, const void*, uint32_t, int32_t);
	void(*DmaClear)(void*, int32_t, uint32_t, int32_t);
	void(*MidiReset)(void);
//	void(*LzUnpack)(int8_t*, int8_t*);
	void(*RleUnpack)(int8_t*, int8_t*, uint32_t);
} IMiscLibrary;

typedef struct IDiskLibrary
{
	int32_t(*OpenFile)(TFileHandle*, const char*, char);
	int32_t(*CloseFile)(TFileHandle*);
	int32_t(*ReadFile)(TFileHandle*,void*,uint32_t);
	int32_t(*WriteFile)(TFileHandle*,void*,uint32_t);
	int32_t(*SeekFile)(TFileHandle*,uint32_t, int32_t);
	int32_t(*TruncateFile)(TFileHandle*);
	int32_t(*FlushFile)(TFileHandle*);
	//char*(*FileReadLine)(TFileHandle*,char*,uint32_t);
	//int32_t(*FilePutChar)(TFileHandle*,char);
	//int32_t(*FileWriteLine)(TFileHandle*,const char*);
	uint32_t(*FilePosition)(TFileHandle*);
	int32_t(*FileEnd)(TFileHandle*);
	uint32_t(*FileSize)(TFileHandle*);
	int32_t(*OpenDir)(TDirHandle*,const char*);
	int32_t(*CloseDir)(TDirHandle*);
	int32_t(*ReadDir)(TDirHandle*,TFileInfo*);
	int32_t(*FindFirst)(TDirHandle*,TFileInfo*,const char*,const char*);
	int32_t(*FindNext)(TDirHandle*,TFileInfo*);
	int32_t(*FileStat)(const char*,TFileInfo*);
	int32_t(*UnlinkFile)(const char*);
	int32_t(*RenameFile)(const char*,const char*);
	int32_t(*FileTouch)(const char*,TFileInfo*);
	int32_t(*MakeDir)(const char*);
	int32_t(*ChangeDir)(const char*);
	int32_t(*GetCurrentDir)(char*, int32_t);
	int32_t(*GetLabel)(char*);
	const char*(*FileErrStr)(int32_t);
} IDiskLibrary;

typedef struct IBios
{
	long AssBang;
	int16_t biosVersion;
	int16_t extensions;
	void(*Exception)(void);
	void(*VBlank)(void);
	void(*HBlank)(void);
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

//----------
//MIDI AIDES
//----------
#define MIDI_PROGRAM(c, p) { REG_MIDIOUT = (0xC0 | c) | (p << 8); }
#define MIDI_CONTROL(c, p, v) { REG_MIDIOUT = (0xB0 | c) | (p << 8) | (v << 16); }
#define MIDI_KEYON(c, p, v) { REG_MIDIOUT = (0x90 | c) | (p << 8) | (v << 16); }
#define MIDI_KEYOFF(c, p, v) { REG_MIDIOUT = (0x80 | c) | (p << 8) | (v << 16); }

#define MIDI_ACOUSTICGRANDPIANO 0
#define MIDI_BRIGHTACOUSTICPIANO 1
#define MIDI_ELECTRICGRANDPIANO 2
#define MIDI_HONKYTONKPIANO 3
#define MIDI_ELECTRICPIANO1 4
#define MIDI_ELECTRICPIANO2 5
#define MIDI_HARPSICHORD 6
#define MIDI_CLAVINET 7
#define MIDI_CELESTA 8
#define MIDI_GLOCKENSPIEL 9
#define MIDI_MUSICBOX 10
#define MIDI_VIBRAPHONE 11
#define MIDI_MARIMBA 12
#define MIDI_XYLOPHONE 13
#define MIDI_TUBULARBELLS 14
#define MIDI_DULCIMER 15
#define MIDI_DRAWBARORGAN 16
#define MIDI_PERCUSSIVEORGAN 17
#define MIDI_ROCKORGAN 18
#define MIDI_CHURCHORGAN 19
#define MIDI_REEDORGAN 20
#define MIDI_ACCORDION 21
#define MIDI_HARMONICA 22
#define MIDI_TANGOACCORDION 23
#define MIDI_ACOUSTICGUITARNYLON 24
#define MIDI_ACOUSTICGUITARSTEEL 25
#define MIDI_ELECTRICGUITARJAZZ 26
#define MIDI_ELECTRICGUITARCLEAN 27
#define MIDI_ELECTRICGUITARMUTED 28
#define MIDI_OVERDRIVENGUITAR 29
#define MIDI_DISTORTIONGUITAR 30
#define MIDI_GUITARHARMONICS 31
#define MIDI_ACOUSTICBASS 32
#define MIDI_ELECTRICBASSFINGER 33
#define MIDI_ELECTRICBASSPICK 34
#define MIDI_FRETLESSBASS 35
#define MIDI_SLAPBASS1 36
#define MIDI_SLAPBASS2 37
#define MIDI_SYNTHBASS1 38
#define MIDI_SYNTHBASS2 39
#define MIDI_VIOLIN 40
#define MIDI_VIOLA 41
#define MIDI_CELLO 42
#define MIDI_CONTRABASS 43
#define MIDI_TREMOLOSTRINGS 44
#define MIDI_PIZZICATOSTRINGS 45
#define MIDI_ORCHESTRALHARP 46
#define MIDI_TIMPANI 47
#define MIDI_STRINGENSEMBLE1 48
#define MIDI_STRINGENSEMBLE2 49
#define MIDI_SYNTHSTRINGS1 50
#define MIDI_SYNTHSTRINGS2 51
#define MIDI_CHOIRAAHS 52
#define MIDI_VOICEOOHS 53
#define MIDI_SYNTHVOICE 54
#define MIDI_ORCHESTRAHIT 55
#define MIDI_TRUMPET 56
#define MIDI_TROMBONE 57
#define MIDI_TUBA 58
#define MIDI_MUTEDTRUMPET 59
#define MIDI_FRENCHHORN 60
#define MIDI_BRASSSECTION 61
#define MIDI_SYNTHBRASS1 62
#define MIDI_SYNTHBRASS2 63
#define MIDI_SOPRANOSAX 64
#define MIDI_ALTOSAX 65
#define MIDI_TENORSAX 66
#define MIDI_BARITONESAX 67
#define MIDI_OBOE 68
#define MIDI_ENGLISHHORN 69
#define MIDI_BASSOON 70
#define MIDI_CLARINET 71
#define MIDI_PICCOLO 72
#define MIDI_FLUTE 73
#define MIDI_RECORDER 74
#define MIDI_PANFLUTE 75
#define MIDI_BLOWNBOTTLE 76
#define MIDI_SHAKUHACHI 77
#define MIDI_WHISTLE 78
#define MIDI_OCARINA 79
#define MIDI_LEAD1SQUARE 80
#define MIDI_LEAD2SAWTOOTH 81
#define MIDI_LEAD3CALLIOPE 82
#define MIDI_LEAD4CHIFF 83
#define MIDI_LEAD5CHARANG 84
#define MIDI_LEAD6VOICE 85
#define MIDI_LEAD7FIFTHS 86
#define MIDI_LEAD8BASSPLUSLEAD 87
#define MIDI_PAD1NEWAGE 88
#define MIDI_PAD2WARM 89
#define MIDI_PAD3POLYSYNTH 90
#define MIDI_PAD4CHOIR 91
#define MIDI_PAD5BOWED 92
#define MIDI_PAD6METALLIC 93
#define MIDI_PAD7HALO 94
#define MIDI_PAD8SWEEP 95
#define MIDI_FX1RAIN 96
#define MIDI_FX2SOUNDTRACK 97
#define MIDI_FX3CRYSTAL 98
#define MIDI_FX4ATMOSPHERE 99
#define MIDI_FX5BRIGHTNESS 100
#define MIDI_FX6GOBLINS 101
#define MIDI_FX7ECHOES 102
#define MIDI_FX8SCIFI 103
#define MIDI_SITAR 104
#define MIDI_BANJO 105
#define MIDI_SHAMISEN 106
#define MIDI_KOTO 107
#define MIDI_KALIMBA 108
#define MIDI_BAGPIPE 109
#define MIDI_FIDDLE 110
#define MIDI_SHANAI 111
#define MIDI_TINKLEBELL 112
#define MIDI_AGOGO 113
#define MIDI_STEELDRUMS 114
#define MIDI_WOODBLOCK 115
#define MIDI_TAIKODRUM 116
#define MIDI_MELODICTOM 117
#define MIDI_SYNTHDRUM 118
#define MIDI_REVERSECYMBAL 119
#define MIDI_GUITARFRETNOISE 120
#define MIDI_BREATHNOISE 121
#define MIDI_SEASHORE 122
#define MIDI_BIRDTWEET 123
#define MIDI_TELEPHONERING 124
#define MIDI_HELICOPTER 125
#define MIDI_APPLAUSE 126
#define MIDI_GUNSHOT 127

#define MIDI_Cn1 0
#define MIDI_Csn1 1
#define MIDI_Dn1 2
#define MIDI_Dsn1 3
#define MIDI_En1 4
#define MIDI_Fn1 5
#define MIDI_Fsn1 6
#define MIDI_Gn1 7
#define MIDI_Gsn1 8
#define MIDI_An1 9
#define MIDI_Asn1 10
#define MIDI_Bn1 11
#define MIDI_C0 12
#define MIDI_Cs0 13
#define MIDI_D0 14
#define MIDI_Ds0 15
#define MIDI_E0 16
#define MIDI_F0 17
#define MIDI_Fs0 18
#define MIDI_G0 19
#define MIDI_Gs0 20
#define MIDI_A0 21
#define MIDI_As0 22
#define MIDI_B0 23
#define MIDI_C1 24
#define MIDI_Cs1 25
#define MIDI_D1 26
#define MIDI_Ds1 27
#define MIDI_E1 28
#define MIDI_F1 29
#define MIDI_Fs1 30
#define MIDI_G1 31
#define MIDI_Gs1 32
#define MIDI_A1 33
#define MIDI_As1 34
#define MIDI_B1 35
#define MIDI_C2 36
#define MIDI_Cs2 37
#define MIDI_D2 38
#define MIDI_Ds2 39
#define MIDI_E2 40
#define MIDI_F2 41
#define MIDI_Fs2 42
#define MIDI_G2 43
#define MIDI_Gs2 44
#define MIDI_A2 45
#define MIDI_As2 46
#define MIDI_B2 47
#define MIDI_C3 48
#define MIDI_Cs3 49
#define MIDI_D3 50
#define MIDI_Ds3 51
#define MIDI_E3 52
#define MIDI_F3 53
#define MIDI_Fs3 54
#define MIDI_G3 55
#define MIDI_Gs3 56
#define MIDI_A3 57
#define MIDI_As3 58
#define MIDI_B3 59
#define MIDI_C4 60
#define MIDI_Cs4 61
#define MIDI_D4 62
#define MIDI_Ds4 63
#define MIDI_E4 64
#define MIDI_F4 65
#define MIDI_Fs4 66
#define MIDI_G4 67
#define MIDI_Gs4 68
#define MIDI_A4 69
#define MIDI_As4 70
#define MIDI_B4 71
#define MIDI_C5 72
#define MIDI_Cs5 73
#define MIDI_D5 74
#define MIDI_Ds5 75
#define MIDI_E5 76
#define MIDI_F5 77
#define MIDI_Fs5 78
#define MIDI_G5 79
#define MIDI_Gs5 80
#define MIDI_A5 81
#define MIDI_As5 82
#define MIDI_B5 83
#define MIDI_C6 84
#define MIDI_Cs6 85
#define MIDI_D6 86
#define MIDI_Ds6 87
#define MIDI_E6 88
#define MIDI_F6 89
#define MIDI_Fs6 90
#define MIDI_G6 91
#define MIDI_Gs6 92
#define MIDI_A6 93
#define MIDI_As6 94
#define MIDI_B6 95
#define MIDI_C7 96
#define MIDI_Cs7 97
#define MIDI_D7 98
#define MIDI_Ds7 99
#define MIDI_E7 100
#define MIDI_F7 101
#define MIDI_Fs7 102
#define MIDI_G7 103
#define MIDI_Gs7 104
#define MIDI_A7 105
#define MIDI_As7 106
#define MIDI_B7 107
#define MIDI_C8 108
#define MIDI_Cs8 109
#define MIDI_D8 110
#define MIDI_Ds8 111
#define MIDI_E8 112
#define MIDI_F8 113
#define MIDI_Fs8 114
#define MIDI_G8 115
#define MIDI_Gs8 116
#define MIDI_A8 117
#define MIDI_As8 118
#define MIDI_B8 119
#define MIDI_C9 120
#define MIDI_Cs9 121
#define MIDI_D9 122
#define MIDI_Ds9 123
#define MIDI_E9 124
#define MIDI_F9 125
#define MIDI_Fs9 126
#define MIDI_G9 127

#define MIDI_BASSDRUM2 35
#define MIDI_BASSDRUM1 36
#define MIDI_SIDESTICK 37
#define MIDI_SNAREDRUM1 38
#define MIDI_HANDCLAP 39
#define MIDI_SNAREDRUM2 40
#define MIDI_LOWTOM2 41
#define MIDI_CLOSEDHIHAT 42
#define MIDI_LOWTOM1 43
#define MIDI_PEDALHIHAT 44
#define MIDI_MIDTOM2 45
#define MIDI_OPENHIHAT 46
#define MIDI_MIDTOM1 47
#define MIDI_HIGHTOM2 48
#define MIDI_CRASHCYMBAL1 49
#define MIDI_HIGHTOM1 50
#define MIDI_RIDECYMBAL1 51
#define MIDI_CHINESECYMBAL 52
#define MIDI_RIDEBELL 53
#define MIDI_TAMBOURINE 54
#define MIDI_SPLASHCYMBAL 55
#define MIDI_COWBELL 56
#define MIDI_CRASHCYMBAL2 57
#define MIDI_VIBRASLAP 58
#define MIDI_RIDECYMBAL2 59
#define MIDI_HIGHBONGO 60
#define MIDI_LOWBONGO 61
#define MIDI_MUTEHIGHCONGA 62
#define MIDI_OPENHIGHCONGA 63
#define MIDI_LOWCONGA 64
#define MIDI_HIGHTIMBALE 65
#define MIDI_LOWTIMBALE 66
#define MIDI_HIGHAGOGO 67
#define MIDI_LOWAGOGO 68
#define MIDI_CABASA 69
#define MIDI_MARACAS 70
#define MIDI_SHORTWHISTLE 71
#define MIDI_LONGWHISTLE 72
#define MIDI_SHORTGUIRO 74
#define MIDI_LONGGUIRO 74
#define MIDI_CLAVES 75
#define MIDI_HIGHWOODBLOCK 76
#define MIDI_LOWWOODBLOCK 77
#define MIDI_MUTECUICA 78
#define MIDI_OPENCUICA 79
#define MIDI_MUTETRIANGLE 80
#define MIDI_OPENTRIANGLE 81

