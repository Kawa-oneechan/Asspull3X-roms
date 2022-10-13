#include "../ass.h"
#include "../ass-midi.h"
#include "../ass-keys.h"

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

void BlankOut();
void DiskEntry();
void Jingle();

bool diskEntryMayRun = false;

int main(void)
{
	char biosVer[32];
	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	bool haveDisk = false, hadDisk = false;
	bool showSplash = false, showMenu = false;
	const char bop[] = { 0, 0, 0, 1, 1, 2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 1 };
	const char blink[] = { 3, 4, 5, 6, 6, 6, 5, 4, 3 };
	int blinker = 0;

	sprintf(biosVer, "BIOS v%d.%d", (interface->biosVersion >> 8) & 0xFF, (interface->biosVersion >> 0) & 0xFF);
	const char banner[] = "\x93\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8B\n\x89 ASSPULL \x11\xD7 \x89\n\x8C\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x92";

	DmaCopy(TEXTFONT, (int8_t*)&fontTiles, 0xC00, DMA_INT);
	ResetPalette();
	MidiReset();
	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	//REG_CARET = 0x8000;

	REG_INTRMODE &= ~IMODE_DISABLE;

	REG_CARET = 80 + 2;
	interface->io.attribs = 0x0B;
	Write("Asspull \x11\xD7 %s\n\n", biosVer);
	((char*)TEXTMAP)[17 + 160 + 4] = 0x0C;
	((char*)TEXTMAP)[19 + 160 + 4] = 0x09;
	interface->io.attribs = 0x07;

#ifdef EXTENSIVE
	{
		REG_CARET = 80 * 3;
		Write("  Memory\n  \x90\x90\x90\x90\x90\x90\n");
		uint8_t* memTest = (uint8_t*)0x01000000;
		while (memTest < (uint8_t*)0x01400000)
		{
			REG_CARET = (80 * 5) + 2;
			Write("%#08X...", memTest);
			*memTest = 42;
			vbl();
			if (*memTest != 42)
			{
				interface->io.attribs = 0x0C;
				Write(" something's up.");
				while (1) vbl();
			}
			memTest += 1024 * 32;
		}
		interface->io.attribs = 0x0A;
		Write(" okay!\n\n");
		interface->io.attribs = 0x07;
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
					if (interface->io.diskToDev[j] == i)
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

	if (interface->io.numDrives == 0)
	{
		Write("  No disk drive connected. Power off, or press F1 to continue.\n\n");
		while (INP_KEYIN != KEYSCAN_F1)
			vbl();
	}
	else if (interface->io.numDrives > 4)
	{
		Write("  Too many disk drives connected. Only the first four will be accessible.\n  Press F1 to continue.\n\n");
		while (INP_KEYIN != KEYSCAN_F1)
			vbl();
	}
#ifdef EXTENSIVE
	else
	{
		WaitForVBlanks(1);
		Jingle();
	}
#endif

	//Fade(false, false);
	diskEntryMayRun = false;

	volatile uint8_t* firstDisk = (uint8_t*)0x02000000 + (interface->io.diskToDev[0] * 0x8000);

	while(true)
	{
goAgain:
//		if (*cartCode != 0x41535321) //ASS!
//		{
			haveDisk = firstDisk[4] & 1;
			if (haveDisk && !hadDisk)
			{
				hadDisk = true;
				FILE file;
				if (OpenFile(&file, "start.cfg", FA_READ) == 0)
				{
					CloseFile(&file);
					OBJECTS_A[0] = OBJECTA_BUILD(32, 0, 1, 0); //disk
					diskEntryMayRun = false;
					DiskEntry();
					//BUT!
					if (*cartCode == 0x41535321 && diskEntryMayRun)
						showMenu = true;
					else
					{
						if (!diskEntryMayRun)
						{
							entry = (void*)0x00020004;
							continue;
						}
						else
							entry = DiskEntry;
						break;
					}
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
//		}
//		else
		if (*cartCode == 0x41535321 && !showMenu) //ASS!
		{
			entry = (void*)0x00020004;
			OBJECTS_A[0] = OBJECTA_BUILD(16, 0, 1, 0); //cart
			break;
		}

		if (INP_KEYIN == KEYSCAN_F1 && showSplash) //F1
		{
doAbout:
			OBJECTS_B[0] = OBJECTB_BUILD(-32, -32, 0, 0, 0, 0, 0, 0);
			char about[256];
			interface->DrawCharFont = (char*)0x0E060C00;
			interface->DrawCharHeight = 0x0808;
			sprintf(about, "%s\nCode by Kawa\n" __DATE__, biosVer);
			for (int i = 2; i <= 6; i++)
			{
				DrawString(banner, 104, 130, i);
				DrawString(about, 104, 154, i);
				WaitForVBlanks(8);
			}
			WaitForVBlanks(8);
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
				DrawString(banner, 104, 130, i);
				DrawString(about, 104, 154, i);
				WaitForVBlanks(8);
			}
			OBJECTS_B[0] = OBJECTB_BUILD(144, 152, 1, 1, 0, 0, 1, 0);
		}

		if (showSplash && showMenu)
		{
			OBJECTS_B[0] = OBJECTB_BUILD(-32, -32, 0, 0, 0, 0, 0, 0);
			interface->DrawCharFont = (char*)0x0E060C00;
			interface->DrawCharHeight = 0x0808;
			const char menu[] = " \x1D CARTRIDGE\n\n   DISKETTE\n";
			DrawString(banner, 104, 130, 6);
			DrawString(menu, 104, 162, 6);
			int cursor = 0;
			while (true)
			{
				int key = INP_KEYIN;
				if (key == KEYSCAN_F1)
				{
					DrawString(banner, 104, 130, 1);
					DrawString(menu, 104, 162, 1);
					DrawString("\x1D", 112, 162, 1);
					goto doAbout;
				}
				if (key == KEYSCAN_UP || key == KEYSCAN_DOWN || (INP_JOYPAD1 && INP_JOYPAD1 <= BUTTON_LEFT))
				{
					while (INP_JOYPAD1) vbl();
					cursor = !cursor;
				}
				if (*cartCode != 0x41535321 || !(firstDisk[4] & 1))
				{
					showMenu = false;
					hadDisk = false;
					haveDisk = false;
					DrawString(banner, 104, 130, 1);
					DrawString(menu, 104, 162, 1);
					OBJECTS_B[0] = OBJECTB_BUILD(144, 152, 1, 1, 0, 0, 1, 0);
					goto goAgain;
				}
				else if (key == KEYSCAN_ENTER || INP_JOYPAD1 >= BUTTON_A)
				{
					if (!cursor)
					{
						entry = (void*)0x00020004;
						OBJECTS_A[0] = OBJECTA_BUILD(16, 0, 1, 0); //cart
					}
					else
					{
						entry = DiskEntry;
						OBJECTS_A[0] = OBJECTA_BUILD(32, 0, 1, 0); //disk
					}
					break;
				}
				DrawString("\x1D", 112, 162 + (cursor * 16), blink[blinker / 8]);
				DrawString("\x1D", 112, 162 + (!cursor * 16), 1);
				blinker++;
				if (blinker == 8 * 8)
					blinker = 0;
				WaitForVBlank();
			}
			DrawString(banner, 104, 130, 1);
			DrawString(menu, 104, 162, 1);
			DrawString("\x1D", 112, 162, 1);
			OBJECTS_B[0] = OBJECTB_BUILD(144, 152, 1, 1, 0, 0, 1, 0);
			break;
		}

		if (!showSplash)
		{
			showSplash = true;
			WaitForVBlank();
			REG_SCREENFADE = 31;
			MISC->DmaClear(TILESET, 0, 0x4000, DMA_INT);
			MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);
			DisplayPicture((TImageFile*)&splashData);
			MISC->DmaCopy(PALETTE + 256, (int8_t*)&iconsPal, 16, DMA_SHORT);
			MISC->DmaCopy(TILESET, (int8_t*)&iconsTiles, 512, DMA_INT);
			OBJECTS_A[0] = OBJECTA_BUILD(0, 0, 1, 0);
			if (!showMenu)
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
	{
		DmaClear((int8_t*)0x01001000, 0, 0x00200000, DMA_INT); //Reset cart's workram
		BlankOut();
	}
	entry();

	showSplash = false;
	showMenu = false;
	goto goAgain;
}

void BlankOut()
{
	MidiReset();
	REG_SCREENMODE = REG_SCREENFADE = REG_MAPSET = 0;
	REG_SCROLLX1 = REG_SCROLLX2 = REG_SCROLLY1 = REG_SCROLLY2 = 0;
	REG_HDMACONTROL[0] = 0;
	REG_HDMACONTROL[1] = 0;
	OBJECTS_A[0] = 0;
	interface->VBlank = 0;
	interface->DrawCharFont = (char*)0x0E060C00;
	interface->DrawCharHeight = 0x0808;
	interface->io.attribs = 0x0F;
	ClearScreen();
	ResetPalette();
}

void DiskEntry()
{
	FILE file;
	FILEINFO nfo;
	char cfgData[512];
	if (OpenFile(&file, "start.cfg", FA_READ)) return;
	ReadFile(&file, cfgData, 512);
	CloseFile(&file);

	char* ptr = cfgData;
	char* key;
	char* value;

	char fontName[16] = { 0 };
	char locName[16] = { 0 };
	char shellName[16] = { 0 };

	while (*ptr != 0)
	{
		if (*ptr == ';')
			while(*ptr != 0 && *ptr != '\n') ptr++;

		//printf("ptr: %s\n", ptr);
		key = ptr;
		while(*ptr != 0 && *ptr != '=') ptr++;
		*ptr = 0;
		ptr++;
		value = ptr;
		while(*ptr != 0 && *ptr != '\n') ptr++;
		*ptr = 0;
		ptr++;

		//printf("key: %s\n", key);
		//printf("value: %s\n", value);
		if (!strncmp(key, "font", 8))
			strcpy(fontName, value);
		else if (!strncmp(key, "locale", 8))
			strcpy(locName, value);
		else if (!strncmp(key, "shell", 8))
			strcpy(shellName, value);
	}

	if (fontName[0])
	{
		FileStat(fontName, &nfo);
		if (nfo.fsize == 12288)
		{
			OpenFile(&file, fontName, FA_READ);
			ReadFile(&file, (void*)TEXTFONT, 0x3000);
		}
		CloseFile(&file);
	}

	if (locName[0])
	{
		FileStat(locName, &nfo);
		if (nfo.fsize == sizeof(TLocale))
		{
			OpenFile(&file, locName, FA_READ);
			ReadFile(&file, (void*)&interface->locale, sizeof(TLocale));
		}
		CloseFile(&file);
	}

//	Write("Shell is \"%s\"", shellName);
//	while (INP_KEYIN != KEYSCAN_F1)
//		vbl();

	if (diskEntryMayRun)
	{
//		Write("Was allowed to run");
//		while (INP_KEYIN != KEYSCAN_F1)
//			vbl();
		if (!shellName[0])
		{
			BlankOut();
			REG_SCREENMODE = SMODE_BOLD | SMODE_240;
			Write("No shell specified. Press F1 to restart.");
			while (INP_KEYIN != KEYSCAN_F1)
				vbl();
			return;
		}
		void(*entry)(void) = (void*)0x01002020;
		FILEINFO nfo;
		FileStat(shellName, &nfo);
		if (OpenFile(&file, shellName, FA_READ))
		{
			BlankOut();
			REG_SCREENMODE = SMODE_BOLD | SMODE_240;
			Write("Could not open \"%s\". Press F1 to restart.", shellName);
			while (INP_KEYIN != KEYSCAN_F1)
				vbl();
			return;
		}
		ReadFile(&file, (void*)0x01002000, nfo.fsize);
		CloseFile(&file);
		entry();
	}
	else if (!shellName[0])
	{
//		Write("Was not allowed to run, nothing TO run anyway");
//		while (INP_KEYIN != KEYSCAN_F1)
//			vbl();
		diskEntryMayRun = false;
	}
	else
	{
//		Write("Was not allowed to run, would've run \"%s\"", shellName);
//		while (INP_KEYIN != KEYSCAN_F1)
//			vbl();
		diskEntryMayRun = true;
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
	interface->io.attribs = 0x0F;
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
	Write("%20s: %#08X", text[which], what);
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
