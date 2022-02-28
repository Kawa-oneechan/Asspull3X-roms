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

void SetTextMode(int32_t flags)
{
	REG_SCREENMODE = SMODE_TEXT | flags;
	interface->DrawChar = 0;
}

void SetBitmapMode16(int32_t flags)
{
	REG_SCREENMODE = SMODE_BMP16 | flags;
	interface->DrawChar = (flags & SMODE_320) ? DrawChar4_320 : DrawChar4_640;
}

void SetBitmapMode256(int32_t flags)
{
	REG_SCREENMODE = SMODE_BMP256 | flags;
	interface->DrawChar = (flags & SMODE_320) ? DrawChar8_320 : DrawChar8_640;
}

void EnableObjects(int32_t enabled)
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

void WaitForVBlanks(int32_t vbls)
{
	while(vbls--) WaitForVBlank();
}

//CONSIDER: Replace these with #define macros in ass.h.
void DmaCopy(void* dst, const void* src, size_t size, int32_t step)
{
	REG_DMASOURCE = (int)src;
	REG_DMATARGET = (int)dst;
	REG_DMALENGTH = size;
	REG_DMACONTROL = 0x07 | (step << 4);
}
void DmaClear(void* dst, int32_t src, size_t size, int32_t step)
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

void RleUnpack(int8_t* dst, int8_t* src, size_t size)
{
	int8_t ints = REG_INTRMODE;
	intoff();

	uint8_t data = 0;
	while (size)
	{
		data = *src++;
		if ((data & 0xC0) == 0xC0)
		{
			int8_t len = data & 0x3F;
			data = *src++;
			size--;
			if (data == 0xC0 && len == 0)
				break;
			for (; len > 0; len--)
				*dst++ = data;
		}
		else
		{
			*dst++ = data;
		}
		size--;
	}

	REG_INTRMODE = ints;
}
