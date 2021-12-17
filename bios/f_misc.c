#include "../ass.h"
#include "funcs.h"

extern int vsprintf(char*, const char*, va_list);

extern int inVblank;

const IMiscLibrary miscLibrary =
{
	SetTextMode, SetBitmapMode16,
	SetBitmapMode256,
	EnableObjects,
	WaitForVBlank, WaitForVBlanks,
	DmaCopy, DmaClear,
	MidiReset,
	RleUnpack
};

void SetTextMode(int flags)
{
	REG_SCREENMODE = SMODE_TEXT | flags;
	interface->DrawChar = 0;
}

void SetBitmapMode16(int flags)
{
	REG_SCREENMODE = SMODE_BMP16 | flags;
	interface->DrawChar = (flags & SMODE_320) ? DrawChar4_320 : DrawChar4_640;
}

void SetBitmapMode256(int flags)
{
	REG_SCREENMODE = SMODE_BMP256 | flags;
	interface->DrawChar = (flags & SMODE_320) ? DrawChar8_320 : DrawChar8_640;
}

void EnableObjects(int enabled)
{
	//To be replaced with RemoveObjects
	enabled = 0;
}

void WaitForVBlank()
{
	long tickCount = REG_TICKCOUNT;
	while (REG_TICKCOUNT == tickCount) { ; }
//	intoff();
//	while(REG_LINE >= 480);
//	while(REG_LINE < 480);
//	if (interface->VBlank)
//		interface->VBlank();
}

void WaitForVBlanks(int vbls)
{
	while(vbls--) WaitForVBlank();
}

//CONSIDER: Replace these with #define macros in ass.h.
void DmaCopy(void* dst, const void* src, uint32_t size, int step)
{
	REG_DMASOURCE = (int)src;
	REG_DMATARGET = (int)dst;
	REG_DMALENGTH = size;
	REG_DMACONTROL = 0x07 | (step << 4);
}
void DmaClear(void* dst, int src, uint32_t size, int step)
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

void RleUnpack(int8_t* dst, int8_t* src, uint32_t size)
{
	REG_BLITSOURCE = (int32_t)src;
	REG_BLITTARGET = (int32_t)dst;
	REG_BLITLENGTH = (int32_t)size;
	REG_BLITCONTROL= BLIT_UNRLE;
}
