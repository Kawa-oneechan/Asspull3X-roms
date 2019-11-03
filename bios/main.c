#include "../ass.h"

#ifdef printf
#undef printf
#define printf Write
#endif

extern int sprintf(char *buf, const char *fmt, ...);

#include "funcs.h"

int inVblank;

//from crt0.s
IBios* interface;

extern int32_t cursorPos;
extern int8_t attribs;
extern int8_t textWidth, textHeight;

extern void cartStart();

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

	int pos = 240 - (strnlen_s(what, 16) * 4); //len * 8, but halved.
	DrawString(what, pos + 1, 77, 50);
	DrawString(what, pos, 76, 62);
}

int32_t main(void)
{
	char biosVer[32];
	interface = (IBios*)(0x01000000);
	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	char* cartName = (char*)0x00020008;
	int32_t haveDisk = 0, hadDisk = 0;
	int32_t showSplash = 0;
	char message[32] = "* INSERT CART *";

	sprintf(biosVer, "BIOS v%d.%d", (interface->biosVersion >> 8) & 0xFF, (interface->biosVersion >> 0) & 0xFF);
	dpf(biosVer);

	DmaCopy((int8_t*)0x0E050200, (int8_t*)&fontTiles, 12288, DMA_INT);

	while(1)
	{
		if (*cartCode != 0x41535321) //ASS!
		{
			haveDisk = *(volatile uint8_t*)(0x02000004) & 1;
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
							strcpy_s(message, 32, "Couldn't read.");
							if (showSplash) Display(message);
							continue;
						}
						ret = CloseFile(&file);
						entry = (void*)0x01002020;
						cartName = (char*)0x01002008;
						break;
					}
					else
					{
						dpf("not good");
						strcpy_s(message, 32, "Not a boot disk.");
						if (showSplash) Display(message);
						continue;
					}
				}
			}
			else if (!haveDisk && hadDisk)
			{
				hadDisk = 0;
				dpf("lost disk");
				strcpy_s(message, 32, "* INSERT CART *");
				if (showSplash) Display(message);
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
			DrawString(biosVer, 2, 2, 50);
			DrawString(biosVer, 1, 1, 1);
			MIDI_PROGRAM(1, MIDI_SEASHORE);
			MIDI_KEYON(1, MIDI_C4, 80);
			Display(message);
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
	if (entry == (void*)0x00020004)
		DmaClear((int8_t*)0x01001000, 0, 0x00200000, DMA_INT); //Reset cart's workram
	MidiReset();
	REG_SCREENMODE = REG_SCREENFADE = REG_MAPSET = 0;
	REG_SCROLLX1 = REG_SCROLLX2 = REG_SCROLLY1 = REG_SCROLLY2 = 0;
	REG_HDMACONTROL[0] = 0;
	interface->VBlank = 0;
	interface->HBlank = 0;
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
	if (interrupts & IMODE_INVBLANK)
	{
		if (interface->VBlank != 0) interface->VBlank();
		REG_INTRMODE &= ~IMODE_INVBLANK;
	}
	else if (interrupts & IMODE_INHBLANK)
	{
		if (interface->HBlank != 0) interface->HBlank();
		REG_INTRMODE &= ~IMODE_INHBLANK;
	}
	asm("rte");
}
