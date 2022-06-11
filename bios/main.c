#include "../ass.h"
#include "../ass-midi.h"

//#define EXTENSIVE

#ifdef printf
#undef printf
#define printf Write
#endif

extern int sprintf(char *buf, const char *fmt, ...);

extern void PrepareDiskToDevMapping();

#include "funcs.h"

//from crt0.s
IBios* interface = (IBios*)(0x01000000);

extern int8_t attribs;

extern char diskToDev[];

extern const uint16_t hdma1[], hdma2[];
extern const uint16_t fontTiles[];
extern const TImageFile splashData;
extern const uint16_t iconsTiles[256];
extern const uint16_t iconsPal[16];

#define OBJECTA_BUILD(t,b,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((b) &  3) <<  9) |		\
	(((t) & 0x1FF) << 0)		\
)
#define OBJECTB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
(												\
	(((pr) & 3) << 29) |						\
	(((ds) & 1) << 28) |						\
	(((vf) & 1) << 27) |						\
	(((hf) & 1) << 26) |						\
	(((dh) & 1) << 25) |						\
	(((dw) & 1) << 24) |						\
	(((vp) & 0x3FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)

void FindFont();
void Jingle();
extern char diskToDev[16];

int main(void)
{
	char biosVer[32];
	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	bool haveDisk = false, hadDisk = false;
	bool showSplash = false;
	const char bop[] = { 0, 0, 0, 1, 1, 2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 1 };

	sprintf(biosVer, "BIOS v%d.%d", (interface->biosVersion >> 8) & 0xFF, (interface->biosVersion >> 0) & 0xFF);

	DmaCopy(TEXTFONT, (int8_t*)&fontTiles, 0xC00, DMA_INT);
	ResetPalette();
	MidiReset();
	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	//REG_CARET = 0x8000;

	inton();

	REG_CARET = 80 + 2;
	attribs = 0x0B;
	Write("Asspull \x96\xD7 %s\n\n", biosVer);
	((char*)TEXTMAP)[17 + 160 + 4] = 0x0C;
	((char*)TEXTMAP)[19 + 160 + 4] = 0x09;
	attribs = 0x07;

#ifdef EXTENSIVE
	{
		REG_CARET = 80 * 3;
		Write("  Memory\n  \x90\x90\x90\x90\x90\x90\n");
		uint8_t* memTest = (uint8_t*)0x01000000;
		while (memTest < (uint8_t*)0x01400000)
		{
			REG_CARET = (80 * 5) + 2;
			Write("0x%08X...", memTest);
			*memTest = 42;
			vbl();
			if (*memTest != 42)
			{
				attribs = 0x0C;
				Write(" something's up.");
				while (1) vbl();
			}
			memTest += 1024 * 32;
		}
		attribs = 0x0A;
		Write(" okay!\n\n");
		attribs = 0x07;
	}
#endif
	PrepareDiskToDevMapping();

#ifdef EXTENSIVE
	{
		REG_CARET = 80 * 7;
		Write("  Devices\n  \x90\x90\x90\x90\x90\x90\x90\n");
		uint8_t* devices = (uint8_t*)0x02000000;
		for (char i = 0; i < 16; i++)
		{
			REG_CARET = (80 * ((i % 8) + 9)) + ((i / 8) * 30) + 2;
			Write("%2d. ", i);
			if (i == 0)
				Write("Input controller");
			else if (*(int16_t*)devices == 0x4C50)
			{
				interface->LinePrinter = devices + 2;
				Write("Line printer");
			}
			else if (*(int16_t*)devices == 0x0144)
			{
				for (int j = 0; j < 4; j++)
				{
					if (diskToDev[j] == i)
					{
						Write("%s drive %c:", *(char*)&devices[5] ? "Hard disk" : "Diskette", 'A' + j);
						break;
					}
				}
			}
			else
				Write("----");
			devices += 0x8000;
		}
	}
#else
	uint8_t* devices = (uint8_t*)0x02000000;
	for (char i = 0; i < 15; i++)
	{
		if (*(int16_t*)devices == 0x4C50)
		{
			interface->LinePrinter = devices + 2;
			break;
		}
		devices += 0x8000;
	}
#endif

	REG_CARET = 80 * 18;

	if (GetNumDrives() == 0)
	{
		Write("  No disk drive connected. Power off, or press F1 to continue.\n\n");
		while (INP_KEYIN != 59)
			vbl();
	}
	else if (GetNumDrives() > 4)
	{
		Write("  Too many disk drives connected. Only the first four will be accessible.\n  Press F1 to continue.\n\n");
		while (INP_KEYIN != 59)
			vbl();
	}
#ifdef EXTENSIVE
	else
	{
		FindFont();
		WaitForVBlanks(1);
		Jingle();
	}
#else
	FindFont();
#endif

	Fade(false, false);

	volatile uint8_t* firstDisk = (uint8_t*)0x02000000 + (diskToDev[0] * 0x8000);

	while(1)
	{
		if (*cartCode != 0x41535321) //ASS!
		{
			haveDisk = firstDisk[4] & 1;
			if (haveDisk && !hadDisk)
			{
				hadDisk = true;
				FILE file;
				if (OpenFile(&file, "start.app", FA_READ) == 0)
				{
					if (ReadFile(&file, (void*)0x01002000, 0) < 0)
					{
						OBJECTS_A[0] = OBJECTA_BUILD(48, 0, 1, 0); //? disk
						continue;
					}
					CloseFile(&file);
					OBJECTS_A[0] = OBJECTA_BUILD(32, 0, 1, 0); //disk
					entry = (void*)0x01002020;
					break;
				}
				else
				{
					OBJECTS_A[0] = OBJECTA_BUILD(48, 0, 1, 0); //? disk
					continue;
				}
			}
			else if (!haveDisk && hadDisk)
			{
				hadDisk = false;
				OBJECTS_A[0] = OBJECTA_BUILD(0, 0, 1, 0); //logo
				continue;
			}
		}
		else
		{
			entry = (void*)0x00020004;
			OBJECTS_A[0] = OBJECTA_BUILD(16, 0, 1, 0); //cart
			break;
		}

		if (INP_KEYIN == 59 && showSplash) //F1
		{
			OBJECTS_B[0] = OBJECTB_BUILD(-32, -32, 0, 0, 0, 0, 0, 0);
			char about[256];
			interface->DrawCharFont = (char*)0x0E060400;
			interface->DrawCharHeight = 0x0808;
			sprintf(about, "\x93\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8B\n\x89 ASSPULL \x96\xD7 \x89\n\x8C\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x92\n%s\nCode by Kawa\n" __DATE__, biosVer);
			for (int i = 2; i <= 6; i++)
			{
				DrawString(about, 104, 130, i);
				WaitForVBlanks(16);
			}
			WaitForVBlanks(128);
			const char blink[] = { 3, 4, 5, 6, 6, 6, 5, 4, 3 };
			int blinker = 0;
			while (!INP_KEYIN)
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
			OBJECTS_B[0] = OBJECTB_BUILD(144, 152, 1, 1, 0, 0, 1, 0);
		}

		if (!showSplash)
		{
			showSplash = true;
			WaitForVBlank();
			REG_SCREENFADE = 31;
			MISC->DmaClear(TILESET, 0, 0x4000, DMA_INT);
			MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);
			DisplayPicture((TImageFile*)&splashData);
			REG_HDMASOURCE[0] = (int32_t)hdma1;
			REG_HDMATARGET[0] = (int32_t)PALETTE;
			REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);
			REG_HDMASOURCE[1] = (int32_t)hdma2;
			REG_HDMATARGET[1] = (int32_t)(PALETTE + 1);
			REG_HDMACONTROL[1] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (254 << 8) | (170 << 20);
			MISC->DmaCopy(PALETTE + 256, (int8_t*)&iconsPal, 16, DMA_SHORT);
			MISC->DmaCopy(TILESET, (int8_t*)&iconsTiles, 512, DMA_INT);
			OBJECTS_A[0] = OBJECTA_BUILD(0, 0, 1, 0);
			OBJECTS_B[0] = OBJECTB_BUILD(144, 152, 1, 1, 0, 0, 1, 0);
			MIDI_PROGRAM(1, MIDI_SEASHORE);
			MIDI_KEYON(1, MIDI_C4, 80);
			Fade(true, false);
		}
		else
		{
			OBJECTS_B[0] = OBJECTB_BUILD(144, 152 + bop[(REG_TICKCOUNT / 16) % 16], 1, 1, 0, 0, 1, 0);
			WaitForVBlank();
		}
		continue;
	}

	if (showSplash)
	{
#ifndef EXTENSIVE
		Jingle();
		MidiReset();
#else
		MidiReset();
		MIDI_PROGRAM(1, MIDI_GUITARFRETNOISE);
		MIDI_KEYON(1, MIDI_C4, 80);
		WaitForVBlanks(256);
#endif
		Fade(false, false);
	}
	if (entry == (void*)0x00020004)
		DmaClear((int8_t*)0x01001000, 0, 0x00200000, DMA_INT); //Reset cart's workram
	MidiReset();
	REG_SCREENMODE = REG_SCREENFADE = REG_MAPSET = 0;
	REG_SCROLLX1 = REG_SCROLLX2 = REG_SCROLLY1 = REG_SCROLLY2 = 0;
	REG_HDMACONTROL[0] = 0;
	REG_HDMACONTROL[1] = 0;
	OBJECTS_A[0] = 0;
	interface->VBlank = 0;
	interface->DrawCharFont = (char*)0x0E060C00;
	interface->DrawCharHeight = 0x0808;
	attribs = 0x0F;
	ClearScreen();
	ResetPalette();
	entry();
}

void FindFont()
{
	FILE file;
	char fontName[] = "A:/font.bin";
	for (int i = 0; i < GetNumDrives(); i++)
	{
		fontName[0] = 'A' + i;
		if (OpenFile(&file, fontName, FA_READ) == 0)
		{
			ReadFile(&file, (void*)TEXTFONT, 0x3000);
			CloseFile(&file);
			return;
		}
	}
}

void Jingle()
{
	MIDI_PROGRAM(1, MIDI_GLOCKENSPIEL);
	MIDI_CONTROL(1, 91, 0);
	MIDI_KEYON(1, MIDI_G6, 100);
	MIDI_CONTROL(1, 93, 0);
	WaitForVBlanks(10);
	MIDI_KEYOFF(1, MIDI_G6, 100);
	MIDI_KEYON(1, MIDI_D6, 100);
	WaitForVBlanks(10);
	MIDI_KEYOFF(1, MIDI_D6, 100);
	MIDI_KEYON(1, MIDI_A6, 100);
	WaitForVBlanks(10);
	MIDI_KEYOFF(1, MIDI_A6, 100);
	MIDI_KEYON(1, MIDI_B6, 100);
	WaitForVBlanks(50);
	//MIDI_KEYOFF(1, MIDI_B6, 100);
}

void ShowException(int which, int what)
{
	const char text[4][20] = {
		"Bus error",
		"Address error",
		"Illegal instruction",
		"Division by zero",
	};
	ClearScreen();
	REG_CARET = 0;
	attribs = 0x0F;
	PALETTE[0] = 0;
	PALETTE[15] = 0x7FFF;
	REG_SCREENMODE = 0x20 | 0x80;
	REG_SCREENFADE = 0;

	WriteChar(0x83);
	for (int i = 0; i < 78; i++)
		WriteChar(0x87);
	WriteChar(0x83);

	WriteChar(0x83);
	REG_CARET = 80 + 40 - 16;
	Write("%20s: 0x%08X", text[which], what);
	REG_CARET = 80 + 80 - 1;
	WriteChar(0x83);

	WriteChar(0x83);
	for (int i = 0; i < 78; i++)
		WriteChar(0x84);
	WriteChar(0x83);

	while(1)
	{
		PALETTE[15] = 0x7FFF;
		WaitForVBlanks(60);
		PALETTE[15] = 0x007F;
		WaitForVBlanks(60);
	}
}
