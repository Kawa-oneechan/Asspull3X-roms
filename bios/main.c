#include "../ass.h"

#ifdef printf
#undef printf
#define printf Write
#endif

extern int sprintf(char *buf, const char *fmt, ...);

#include "funcs.h"

//from crt0.s
IBios* interface = (IBios*)(0x01000000);

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

int32_t main(void)
{
	char biosVer[32];
	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	int32_t haveDisk = 0, hadDisk = 0;
	int32_t showSplash = 0;

	sprintf(biosVer, "BIOS v%d.%d", (interface->biosVersion >> 8) & 0xFF, (interface->biosVersion >> 0) & 0xFF);
	dpf(biosVer);

	DmaCopy(TEXTFONT, (int8_t*)&fontTiles, 12288, DMA_INT);
	REG_CARET = 0x8000;

	while(1)
	{
		if (*cartCode != 0x41535321) //ASS!
		{
			haveDisk = *(volatile uint8_t*)(0x02000004) & 1;
			if (haveDisk && !hadDisk)
			{
				hadDisk = 1;
				FILE file;
				if (OpenFile(&file, "0:/start.app", FA_READ) == 0)
				{
					if (ReadFile(&file, (void*)0x01002000, 0) < 0)
					{
						SPRITES_A[0] = SPRITEA_BUILD(48, 1, 1); //? disk
						continue;
					}
					CloseFile(&file);
					SPRITES_A[0] = SPRITEA_BUILD(32, 1, 1); //disk
					entry = (void*)0x01002020;
					break;
				}
				else
				{
					dpf("not good");
					SPRITES_A[0] = SPRITEA_BUILD(48, 1, 1); //? disk
					continue;
				}
			}
			else if (!haveDisk && hadDisk)
			{
				hadDisk = 0;
				dpf("lost disk");
				SPRITES_A[0] = SPRITEA_BUILD(0, 1, 1); //logo
				continue;
			}
		}
		else
		{
			entry = (void*)0x00020004;
			SPRITES_A[0] = SPRITEA_BUILD(16, 1, 1); //cart
			break;
		}

		if (REG_KEYIN == 59 && showSplash) //F1
		{
			SPRITES_B[0] = SPRITEB_BUILD(-32, -32, 0, 0, 0, 0, 0, 0);
			char about[256];
			interface->DrawCharFont = (char*)0x0E060A00;
			interface->DrawCharHeight = 0x0808;
			sprintf(about, "  ASSPULL \x96\xD7\n\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\n%s\nCode by Kawa\n" __DATE__, biosVer);
			for (int i = 2; i <= 6; i++)
			{
				DrawString(about, 104, 130, i);
				WaitForVBlanks(16);
			}
			WaitForVBlanks(128);
			const char blink[] = { 3, 4, 5, 6, 6, 6, 5, 4, 3 };
			int blinker = 0;
			while (!REG_KEYIN)
			{
				if (blinker % 8 == 0)
					DrawString("\x03", 208, 202, blink[blinker / 8]);
				blinker++;
				if (blinker == 8 * 8)
					blinker = 0;
				WaitForVBlank();
			}
			DrawString("\x03", 208, 202, 1);
			for (int i = 5; i >= 1; i--)
			{
				DrawString(about, 104, 130, i);
				WaitForVBlanks(16);
			}
			SPRITES_B[0] = SPRITEB_BUILD(144, 152, 1, 1, 0, 0, 1, 0);
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
			MISC->DmaCopy(TILESET, (int8_t*)&iconsTiles, 512, DMA_INT);
			SPRITES_A[0] = SPRITEA_BUILD(0, 1, 1);
			SPRITES_B[0] = SPRITEB_BUILD(144, 152, 1, 1, 0, 0, 1, 0);
			MIDI_PROGRAM(1, MIDI_SEASHORE);
			MIDI_KEYON(1, MIDI_C4, 80);
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
		WaitForVBlanks(256);
		FadeToBlack();
	}
	if (entry == (void*)0x00020004)
		DmaClear((int8_t*)0x01001000, 0, 0x00200000, DMA_INT); //Reset cart's workram
	MidiReset();
	REG_SCREENMODE = REG_SCREENFADE = REG_MAPSET = 0;
	REG_SCROLLX1 = REG_SCROLLX2 = REG_SCROLLY1 = REG_SCROLLY2 = 0;
	REG_HDMACONTROL[0] = 0;
	REG_HDMACONTROL[1] = 0;
	SPRITES_A[0] = 0;
	interface->VBlank = 0;
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
	REG_CARET = 0;
	printf("Fucked up, yo.");
	while(1) WaitForVBlank();
	asm("rte");
}

void AddressHandler()
{
	attribs = 0x4E;
	REG_CARET = 0;
	printf("Address error!");
	while(1) WaitForVBlank();
	asm("rte");
}

void InstructionHandler()
{
	attribs = 0x4E;
	REG_CARET = 0;
	printf("Illegal mangrasp!");
	while(1) WaitForVBlank();
	asm("rte");
}

void ZeroHandler()
{
	attribs = 0x4E;
	REG_CARET = 0;
	printf("Division by zero. Wow.");
	while(1) WaitForVBlank();
	asm("rte");
}

void NMIHandler()
{
	int interrupts = REG_INTRMODE;
	if (interrupts & IMODE_INVBLANK)
	{
		REG_INTRMODE &= ~IMODE_INVBLANK;
		if (interface->VBlank != 0) interface->VBlank();
	}
	asm("rte");
}
