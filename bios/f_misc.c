#include "../ass.h"
#include "funcs.h"

extern int vsprintf(char*, const char*, va_list);
extern int cursorPos;
extern int8_t attribs, textWidth, textHeight;

extern int inVblank;

const IMiscLibrary miscLibrary =
{
	SetTextMode, SetBitmapMode16,
	SetBitmapMode256,
	EnableSprites,
	WaitForVBlank, WaitForVBlanks,
	DmaCopy, DmaClear,
	MidiReset,
	RleUnpack
};

void SetTextMode(int flags)
{
	REG_SCREENMODE = SMODE_TEXT | flags;
	textWidth = (flags & SMODE_320) ? 40 : 80;
	textHeight = (flags & SMODE_240) ? 30 : 60;
	interface->DrawChar = 0;
}

void SetBitmapMode16(int flags)
{
	REG_SCREENMODE = SMODE_BMP1 | flags;
	textWidth = (flags & SMODE_320) ? 40 : 80;
	textHeight = (flags & SMODE_240) ? 30 : 60;
	interface->DrawChar = (flags & SMODE_320) ? DrawChar4_320 : DrawChar4_640;
}

void SetBitmapMode256(int flags)
{
	REG_SCREENMODE = SMODE_BMP2 | flags;
	textWidth = (flags & SMODE_320) ? 40 : 80;
	textHeight = (flags & SMODE_240) ? 30 : 60;
	interface->DrawChar = (flags & SMODE_320) ? DrawChar8_320 : DrawChar8_640;
}

void EnableSprites(int enabled)
{
	//To be replaced with RemoveSprites
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

void WaitForVBlanks(int i)
{
	while(i--) WaitForVBlank();
}

void DmaCopy(void* dst, const void* src, uint32_t size, int step)
{
	//MIDI_KEYON(4, 32, 100);
	REG_DMASOURCE = (int)src;
	REG_DMATARGET = (int)dst;
	REG_DMALENGTH = size;
	REG_DMACONTROL = 0x07 | (step << 4);
}
void DmaClear(void* dst, int src, uint32_t size, int step)
{
	//MIDI_KEYON(4, 64, 100);
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
