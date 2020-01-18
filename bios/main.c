#include "../ass.h"

#ifdef printf
#undef printf
#define printf Write
#endif

extern int sprintf(char *buf, const char *fmt, ...);

#include "funcs.h"

//from crt0.s
IBios* interface;

extern int32_t cursorPos;
extern int8_t attribs;

extern const uint16_t hdma1[], hdma2[];
extern const uint16_t fontTiles[];
extern const TImageFile splashData;
extern const uint16_t iconsTiles[256];
extern const uint16_t iconsPal[16];

#define SPRITEA_BUILD(t,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((t) & 0x1FF) << 0)		\
)
#define SPRITEB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
(												\
	(((pr) & 3) << 30) |						\
	(((ds) & 1) << 28) |						\
	(((vf) & 1) << 27) |						\
	(((hf) & 1) << 26) |						\
	(((dh) & 1) << 25) |						\
	(((dw) & 1) << 24) |						\
	(((vp) & 0x7FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)

/*
void Display(char* what)
{
	//TODO: Make some nice #define macros for the blitter, in ass.h.
	REG_BLITSOURCE = 0;
	REG_BLITTARGET = 0x0E000000 + (76 * 320) + 170;
	REG_BLITLENGTH = 280 * 2;
	REG_BLITCONTROL = BLIT_SET | BLIT_INT | BLIT_STRIDESKIP | BLIT_SOURCESTRIDE(35) | BLIT_TARGETSTRIDE(80);

	int pos = 240 - (strnlen_s(what, 16) * 4); //len * 8, but halved.
	DrawString(what, pos + 1, 77, 50);
	DrawString(what, pos, 76, 62);
}
*/

int32_t main(void)
{
	char biosVer[32];
	interface = (IBios*)(0x01000000);
	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	//char* cartName = (char*)0x00020008;
	int32_t haveDisk = 0, hadDisk = 0;
	int32_t showSplash = 0;
	//char message[32] = "\x14 INSERT CART \x15";

	sprintf(biosVer, "BIOS v%d.%d", (interface->biosVersion >> 8) & 0xFF, (interface->biosVersion >> 0) & 0xFF);
	dpf(biosVer);

	DmaCopy(TEXTFONT, (int8_t*)&fontTiles, 12288, DMA_INT);

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
				FILE file;
				if (OpenFile(&file, "0:/start.app", FA_READ) == 0)
				{
					dpf("got start.app");
					if (ReadFile(&file, (void*)0x01002000, 0) < 0)
					{
						//strcpy_s(message, 32, "Couldn't read.");
						//if (showSplash) Display(message);
						SPRITES_A[0] = SPRITEA_BUILD(48, 1, 1); //? disk
						continue;
					}
					CloseFile(&file);
					SPRITES_A[0] = SPRITEA_BUILD(32, 1, 1); //disk
					entry = (void*)0x01002020;
					//cartName = (char*)0x01002008;
					break;
				}
				else
				{
					dpf("not good");
					//strcpy_s(message, 32, "Not a boot disk.");
					//if (showSplash) Display(message);
					SPRITES_A[0] = SPRITEA_BUILD(48, 1, 1); //? disk
					continue;
				}
			}
			else if (!haveDisk && hadDisk)
			{
				hadDisk = 0;
				dpf("lost disk");
				//strcpy_s(message, 32, "\x14 INSERT CART \x15");
				//if (showSplash) Display(message);
				SPRITES_A[0] = SPRITEA_BUILD(0, 0, 1); //logo
				continue;
			}
		}
		else
		{
			entry = (void*)0x00020004;
			SPRITES_A[0] = SPRITEA_BUILD(16, 1, 1); //cart
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
			REG_HDMASOURCE[1] = (int32_t)hdma2;
			REG_HDMATARGET[1] = (int32_t)(PALETTE + 1);
			REG_HDMACONTROL[1] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (254 << 8) | (170 << 20);
			MISC->DmaCopy(PALETTE + 16, (int8_t*)&iconsPal, 16, DMA_INT);
			MISC->DmaCopy(TILESET, (int8_t*)&iconsTiles, 384, DMA_INT);
			SPRITES_A[0] = SPRITEA_BUILD(0, 1, 1);
			SPRITES_B[0] = SPRITEB_BUILD(144, 152, 1, 1, 0, 0, 1, 0);
//			interface->DrawCharFont = (char*)0x0E060A00;
//			interface->DrawCharHeight = 0x0808;
//			DrawString(biosVer, 2, 2, 50);
//			DrawString(biosVer, 1, 1, 1);
			MIDI_PROGRAM(1, MIDI_SEASHORE);
			MIDI_KEYON(1, MIDI_C4, 80);
//			interface->DrawCharFont = (char*)0x0E062200;
//			interface->DrawCharHeight = 0x1010;
//			Display(message);
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
//		Display(cartName);
		WaitForVBlanks(256);
		FadeToBlack();
		SPRITES_A[0] = 0;
		SPRITES_B[0] = SPRITEB_BUILD(-16, -16, 0, 0, 0, 0, 0, 0);
	}
	if (entry == (void*)0x00020004)
		DmaClear((int8_t*)0x01001000, 0, 0x00200000, DMA_INT); //Reset cart's workram
	MidiReset();
	REG_SCREENMODE = REG_SCREENFADE = REG_MAPSET = 0;
	REG_SCROLLX1 = REG_SCROLLX2 = REG_SCROLLY1 = REG_SCROLLY2 = 0;
	REG_HDMACONTROL[0] = 0;
	interface->VBlank = 0;
	interface->HBlank = 0;
	interface->DrawCharFont = (char*)0x0E060A00;
	interface->DrawCharHeight = 0x0808;
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
