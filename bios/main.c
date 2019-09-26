#include "../ass.h"

#ifdef printf
#undef printf
#define printf Write
#endif

extern int sprintf(char *buf, const char *fmt, ...);

#include "funcs.h"

int inVblank;

//from crt0.s
extern IBios interface;

extern int32_t cursorPos;
extern int8_t attribs;
extern int8_t textWidth, textHeight;

extern void cartStart();
extern IBios interface;

extern const uint16_t hdma1[], hdma2[];
extern const uint16_t fontTiles[];
extern const TImageFile splashData;

void Display(char* what)
{
	for (int y = 76; y < 84; y++)
		DmaClear((void*)0x0E000000 + (y * 160) + 86, 0, 35, DMA_SHORT);
	int pos = 240 - (strnlen_s(what, 16) * 4); //len * 8, but halved.
	DrawString(what, pos + 1, 77, 9);
	DrawString(what, pos, 76, 15);
}

int32_t main(void)
{
	char biosVer[32];
	WaitForVBlank();
	REG_SCREENFADE = 31;
	DisplayPicture((TImageFile*)&splashData);
	DmaCopy((int8_t*)0x0E100200, (int8_t*)&fontTiles, 12288, DMA_INT);
	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);
	sprintf(biosVer, "BIOS v%d.%d", (interface.biosVersion >> 8) & 0xFF, (interface.biosVersion >> 0) & 0xFF);
	dpf(biosVer);
	DrawString(biosVer, 2, 2, 9);
	DrawString(biosVer, 1, 1, 14);
	MIDI_PROGRAM(1, MIDI_SEASHORE);
	MIDI_KEYON(1, MIDI_C4, 80);
	Display("* INSERT CART *");
	FadeFromBlack();

	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	char* cartName = (char*)0x00020008;
	int32_t haveDisk;
	while(1)
	{
		if (*cartCode != 0x41535321) //ASS!
		{
			haveDisk = *(volatile uint8_t*)(0x0D800032) & 1;
			if (haveDisk)
			{
				//We got a disk!
				DIR dir;
				FILEINFO info;
				FILE file;
				int32_t ret = FindFirst(&dir, &info, "0:/", "start.app");
				CloseDir(&dir);
				if (ret == 0 && info.fname[0])
				{
					ret = OpenFile(&file, "0:/start.app", FA_READ);
					if (ret)
					{
						Display("Not a boot disk.");
						continue;
					}
					ret = ReadFile(&file, (void*)0x01002000, info.fsize);
					if (ret < 0)
					{
						Display("Failed to read. ");
						continue;
					}
					ret = CloseFile(&file);
					entry = (void*)0x01002020;
					cartName = (char*)0x01002008;
					goto loadIt;
				}
			}

			WaitForVBlank();
			continue;
		}
		else
		{
			entry = (void*)0x00020004;
			Display("* INSERT  CART *");
		}


loadIt:
		MidiReset();

		MIDI_PROGRAM(1, MIDI_GUITARFRETNOISE);
		MIDI_KEYON(1, MIDI_C4, 80);

		Display(cartName);
		FadeToBlack();
		REG_HDMACONTROL[0] = 0;
		interface.VBlank = 0;
		interface.HBlank = 0;
		attribs = 0x0F;
		ClearScreen();
		ResetPalette();
		REG_SCREENFADE = 0;
		entry();
	}
}

void ExHandler()
{
	attribs = 0x4E;
	cursorPos = 0;
	printf("Fucked up, yo.");
	while(1) WaitForVBlank();
	asm("rte");
}

void AddressHandler()
{
	attribs = 0x4E;
	cursorPos = 0;
	printf("Address error!");
	while(1) WaitForVBlank();
	asm("rte");
}

void InstructionHandler()
{
	attribs = 0x4E;
	cursorPos = 0;
	printf("Illegal mangrasp!");
	while(1) WaitForVBlank();
	asm("rte");
}

void ZeroHandler()
{
	attribs = 0x4E;
	cursorPos = 0;
	printf("Division by zero. Wow.");
	while(1) WaitForVBlank();
	asm("rte");
}

void NMIHandler()
{
	int interrupts = REG_INTRMODE;
	if (interrupts & 4)
	{
		if (interface.VBlank != 0) interface.VBlank();
		interrupts &= ~4;
	}
	else if (interrupts & 2)
	{
		if (interface.HBlank != 0) interface.HBlank();
		interrupts &= ~2;
	}
	asm("rte");
}
