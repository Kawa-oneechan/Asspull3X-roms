#include "../ass.h"
#include "funcs.h"

extern IBios interface;
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
	interface.DrawChar = 0;
}

void SetBitmapMode16(int flags)
{
	REG_SCREENMODE = SMODE_BMP1 | flags;
	textWidth = (flags & SMODE_320) ? 40 : 80;
	textHeight = (flags & SMODE_240) ? 30 : 60;
	interface.DrawChar = (flags & SMODE_320) ? DrawChar4_320 : DrawChar4_640;
}

void SetBitmapMode256(int flags)
{
	REG_SCREENMODE = SMODE_BMP2 | flags;
	textWidth = (flags & SMODE_320) ? 40 : 80;
	textHeight = (flags & SMODE_240) ? 30 : 60;
	//interface.DrawChar = (flags & SMODE_320) ? DrawChar8_320 : DrawChar8_640;
	interface.DrawChar = DrawChar8_320;
}

void EnableSprites(int enabled)
{
	if (enabled)
		REG_SCREENMODE |= SMODE_SPRITES;
	else
		REG_SCREENMODE &= ~SMODE_SPRITES;
}

void WaitForVBlank()
{
	long tickCount = REG_TICKCOUNT;
	while (REG_TICKCOUNT == tickCount) { ; }
//	REG_INTRMODE |= 0x80;
//	while(REG_LINE >= 480);
//	while(REG_LINE < 480);
//	if (interface.VBlank)
//		interface.VBlank();
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

/*
void LzUnpack(char* dst, char *src)
{
	if (*src++ != 0x10)
		return; //Not LZ77 type 0x10 compressed!
	int size = *src++ << 0;
	size |= *src++ << 8;
	size |= *src++ << 16;
	for (int i = 0; i < size; i++)
	{
		byte b = *src++;
		for (int j = 0; j < 8; j++)
		{
			if (b & 0x80)
			{
				int num2 = 3 + (*src >> 4);
				int num3 = 1;
				num3 += ((*src++) & 0x0F) << 8;
				num3 += (*src++);
				if (num3 > size) return;
				for (int k = 0; k < num2; k++)
				{
					dst[i] = dst[j - k - num3 + k % num3];
					i++;
				}
			}
			else
				dst[i++] = *src++;
			if (i >= size) break;
			b <<= 1;
		}
	}
}
*/
/*
void LzUnpack(char* dst, char* src)
{
	src++;
	int len = *src++ << 16;
	len |= *src++ << 8;
	len |= *src++ << 0;
	int mask = 1;
	int flags = 0;
	while (len > 0)
	{
		if (mask == 1)
		{
			flags = *src++;
			mask = 0x80;
		}
		else
			mask >>= 1;

		if ((flags & mask) > 0)
		{
			int data = *src++ << 8;
			data |= *src++ << 0;
			int length = (data >> 12) + 3;
			int offset = (data & 0x0FFF);
			char* windowOffset = dst - offset - 1;
			for (int i = 0; i < length; i++)
			{
				*dst++ = *windowOffset++;
				len--; if(len == 0) return;
			}
		}
		else
		{
			*dst++ = *src++;
			len--; if(len == 0) return;
		}
	}
}
*/


void RleUnpack(int8_t* dst, int8_t* src, uint32_t size)
{
	REG_INTRMODE |= 0x80;
	int8_t data;
	uint8_t rle;
	dpf("RleUnpack: %d bytes from %x to %x", size, src, dst);
	while (size > 0)
	{
		rle = *src++;
		rle++;
		data = *src++;
		for (; rle > 0; rle--)
		{
			size--;
			*dst++ = data;
		}
	}
	REG_INTRMODE &= ~0x80;
/*
	src++;
	int len = *src << 16;
	len |= *src << 8;
	len |= *src << 0;
	while (len > 0)
	{
		char d = *src++;
		int l = d & 0x7F;
		if (d & 0x80)
		{
			char data = *src++;
			l += 3;
			for (int i = 0; i < l; i++)
			{
				*dst++ = data;
				len--;
				if(len == 0)
				{
					REG_INTRMODE &= ~0x80;
					return;
				}
			}
		}
		else
		{
			l++;
			for(int i = 0; i < l; i++)
			{
				*dst++ = *src++;
				len--;
				if(len == 0)
				{
					REG_INTRMODE &= ~0x80;
					return;
				}
			}
		}
	}
*/
}
