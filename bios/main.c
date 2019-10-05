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
	//for (int y = 76; y < 84; y++)
	//	DmaClear((void*)0x0E000000 + (y * 320) + 170, 0, 70, DMA_SHORT);
	REG_BLITSOURCE = 0;
	REG_BLITTARGET = 0x0E000000 + (76 * 320) + 170;
	REG_BLITLENGTH = 280;
	REG_BLITCONTROL = BLIT_SET | BLIT_INT | BLIT_STRIDESKIP | BLIT_SOURCESTRIDE(35) | BLIT_TARGETSTRIDE(80);
	//(80 << 20) | (35 << 8) | (2 << 5) | (1 << 4) | (2 << 0);

	int pos = 240 - (strnlen_s(what, 16) * 4); //len * 8, but halved.
	DrawString(what, pos + 1, 77, 50);
	DrawString(what, pos, 76, 62);
}

int32_t main(void)
{
	char biosVer[32];
	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	char* cartName = (char*)0x00020008;
	int32_t haveDisk = 0, hadDisk = 0;
	int32_t showSplash = 0;

	DmaClear((int8_t*)0x01001000, 0, 0x003F0000, DMA_INT); //Reset cart's workram

	DmaCopy((int8_t*)0x0E100200, (int8_t*)&fontTiles, 12288, DMA_INT);

	while(1)
	{
		if (*cartCode != 0x41535321) //ASS!
		{
			haveDisk = *(volatile uint8_t*)(0x0D800032) & 1;
			if (haveDisk && !hadDisk)
			{
				hadDisk = 1;
				dpf("got disk");
				//We got a disk!
				DIR dir;
				FILEINFO info;
				FILE file;
				int32_t ret = FindFirst(&dir, &info, "0:/", "start.app");
				CloseDir(&dir);
				if (ret == 0)
				{
					if (info.fname[0])
					{
						dpf("got start.app");
						ret = OpenFile(&file, "0:/start.app", FA_READ);
						ret = ReadFile(&file, (void*)0x01002000, info.fsize);
						if (ret < 0)
						{
							if (showSplash) Display("Failed to read. ");
							continue;
						}
						ret = CloseFile(&file);
						entry = (void*)0x01002020;
						cartName = (char*)0x01002008;
						break;
					}
				}
				else
				{
					dpf("not good");
					if (showSplash) Display("Not a boot disk.");
					continue;
				}
			}
			else if (!haveDisk && hadDisk)
			{
				hadDisk = 0;
				dpf("lost disk");
				Display("* INSERT CAT *");
				continue;
			}
		}
		else
		{
			entry = (void*)0x00020004;
			break;
		}

		if (!showSplash)
		{
			showSplash = 1;
			WaitForVBlank();
			REG_SCREENFADE = 31;
			DisplayPicture((TImageFile*)&splashData);
			REG_HDMASOURCE[0] = (int32_t)hdma1;
			REG_HDMATARGET[0] = (int32_t)PALETTE;
			REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);
			sprintf(biosVer, "BIOS v%d.%d", (interface.biosVersion >> 8) & 0xFF, (interface.biosVersion >> 0) & 0xFF);
			dpf(biosVer);
			DrawString(biosVer, 2, 2, 50);
			DrawString(biosVer, 1, 1, 1);
			MIDI_PROGRAM(1, MIDI_SEASHORE);
			MIDI_KEYON(1, MIDI_C4, 80);
			Display("* INSERT CART *");
			FadeFromBlack();
		}
		else
		{
			WaitForVBlank();
		}
		continue;
	}

	if (showSplash)
	{
		MidiReset();
		MIDI_PROGRAM(1, MIDI_GUITARFRETNOISE);
		MIDI_KEYON(1, MIDI_C4, 80);
		Display(cartName);
		FadeToBlack();
	}
	REG_HDMACONTROL[0] = 0;
	REG_SCREENFADE = 0;
	interface.VBlank = 0;
	interface.HBlank = 0;
	attribs = 0x0F;
	ClearScreen();
	ResetPalette();
	entry();
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
