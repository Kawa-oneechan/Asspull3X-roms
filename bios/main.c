#include "../ass.h"
#include "../ass-midi.h"
#include "../ass-keys.h"

#ifdef printf
#undef printf
#define printf Write
#endif

extern int strncmp(const char *l, const char *r, size_t max);
extern int sprintf(char *buf, const char *fmt, ...);

extern void PrepareDiskToDevMapping();

#include "funcs.h"

//from crt0.s
IBios* interface = (IBios*)(0x01000000);

extern const uint16_t fontTiles[];
extern const TPicFile splashData;
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
void LoadConfig(int);
void Jingle();
bool AnyDisks();

extern void Navigator();

uint16_t key;
uint32_t ssTicks;
extern void Starfield();
void ScreenSaverTick()
{
	if (key)
		ssTicks = 0;

	ssTicks++;
	if (ssTicks > 60 * 60)
	{
		interface->vBlank = 0;
		Starfield();
		ssTicks = 0;
		interface->vBlank = ScreenSaverTick;
	}
}

//extern int Kilo(char* file);

#pragma GCC diagnostic ignored "-Wmain"
__attribute__ ((noreturn))
void main(void)
{
	char biosVer[32];
	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	bool showSplash = false;
	static const char bop[] = { 0, 0, 0, 1, 1, 2, 3, 3, 4, 4, 4, 3, 3, 2, 1, 1 };
	static const char blink[] = { 9, 7, 4, 4, 4, 4, 7, 9, 1 };
	static const char fade[] = { 1, 1, 9, 7, 4 };
	int blinker = 0;

	sprintf(biosVer, "BIOS v%d.%d", (interface->biosVersion >> 8) & 0xFF, (interface->biosVersion >> 0) & 0xFF);
	const char banner[] = "\x93\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8B\n\x89 ASSPULL \x11\xD7 \x89\n\x8C\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x92";

	DmaCopy(TEXTFONT, (int8_t*)&fontTiles, 0xC00, DMA_INT);
	ResetPalette();
	MidiReset();
	OplReset();
	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	//REG_CARET = 0x8000;

	REG_INTRMODE &= ~IMODE_DISABLE;

	REG_CARET = 80 + 2;
	interface->io.attribs = 0x0B;
	Write("Asspull \x11\xD7 %s\n\n", biosVer);
	((char*)TEXTMAP)[17 + 160 + 4] = 0x0C;
	((char*)TEXTMAP)[19 + 160 + 4] = 0x09;
	interface->io.attribs = 0x07;

	PrepareDiskToDevMapping();

	uint8_t* devices = (uint8_t*)MEM_DEVS;
	for (char i = 0; i < 15; i++)
	{
		if (*(int16_t*)devices == DEVICE_ID_LINEPRINTER)
		{
			interface->linePrinter = devices + 2;
			break;
		}
		devices += 0x8000;
	}

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

	//Fade(false, false);

	for (int i = 0; i < interface->io.numDrives; i++)
	{
		volatile uint8_t* firstDisk = (uint8_t*)MEM_DEVS + (interface->io.diskToDev[i] * 0x8000);
		if (firstDisk[4] & 1)
		{
			LoadConfig(i);
			break;
		}
	}

//	ShowText("A:/README.TXT");
//	Kilo("A:/README.TXT");
//	Navigator();

	ssTicks = 0;
	interface->vBlank = ScreenSaverTick;
	while(true)
	{
goAgain:
		if (*cartCode == CARTRIDGE_MARKER)
		{
			entry = (void*)0x00020004;
			OBJECTS_A[0] = OBJECTA_BUILD(16, 0, 1, 0); //cart
			break;
		}

		key = INP_KEYIN;
		if (key == KEYSCAN_F1 && showSplash)
		{
			OBJECTS_B[0] = OBJECTB_BUILD(-32, -32, 0, 0, 0, 0, 0, 0);
			char about[256];
			interface->drawCharFont = (char*)TEXTFONT_BOLD8;
			interface->drawCharHeight = 0x0808;
			sprintf(about, "%s\nCode by Kawa\n" __DATE__, biosVer);
			for (unsigned int i = 0; i < array_size(fade); i++)
			{
				DrawString(banner, 48, 64, fade[i]);
				DrawString(about, 48, 88, fade[i]);
				WaitForVBlanks(8);
			}
			WaitForVBlanks(8);
			while (!(key = INP_KEYIN))
			{
				if (blinker % 8 == 60)
					DrawString("\x03", 144, 136, blink[blinker / 8]);
				blinker++;
				if (blinker == 8 * 8)
					blinker = 0;
				WaitForVBlank();
			}
			DrawString("\x03", 144, 136, 1);
			for (unsigned int i = array_size(fade) - 1; i > 0; i--)
			{
				DrawString(banner, 48, 64, fade[i]);
				DrawString(about, 48, 88, fade[i]);
				WaitForVBlanks(8);
			}
			OBJECTS_B[0] = OBJECTB_BUILD(88, 88, 1, 1, 0, 0, 1, 0);
		}
		else if (key == KEYSCAN_ENTER && showSplash)
		{
			if (AnyDisks())
			{
				OBJECTS_A[0] = OBJECTA_BUILD(32, 0, 1, 0);
				entry = Navigator;
				WaitForVBlanks(128);
				break;
			}
			else
			{
				OBJECTS_A[0] = OBJECTA_BUILD(48, 0, 1, 0);
				WaitForVBlanks(128);
				OBJECTS_A[0] = OBJECTA_BUILD(0, 0, 1, 0);
			}
		}

		if (!showSplash)
		{
			showSplash = true;
			WaitForVBlank();
			REG_SCREENFADE = 31;
			DmaClear(TILESET, 0, 0x4000, DMA_INT);
			DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);
			DisplayPicture(&splashData);
			SetupDrawChar(0);
			DmaCopy(PALETTE + 256, (int8_t*)&iconsPal, 16, DMA_SHORT);
			DmaCopy(TILESET, (int8_t*)&iconsTiles, 512, DMA_INT);
			OBJECTS_A[0] = OBJECTA_BUILD(0, 0, 1, 0);
			OBJECTS_B[0] = OBJECTB_BUILD(88, 88, 1, 1, 0, 0, 1, 0);
			MIDI_PROGRAM(1, MIDI_SEASHORE);
			MIDI_KEYON(1, MIDI_C4, 80);
			Fade(true, false);
		}
		else
		{
			OBJECTS_B[0] = OBJECTB_BUILD(88, 88 + bop[(REG_TICKCOUNT / 16) % 16], 1, 1, 0, 0, 1, 0);
			WaitForVBlank();
		}
		continue;
	}

	if (showSplash)
	{
		if (entry != Navigator)
			Jingle();
		MidiReset();
		OplReset();
		Fade(false, false);
	}

	DmaClear((int8_t*)0x01001000, 0, 0x00080000 / 4, DMA_INT); //Reset cart's workram
	BlankOut();
	entry();

	showSplash = false;
	goto goAgain;
}

void BlankOut()
{
	MidiReset();
	OplReset();
	REG_SCREENMODE = REG_SCREENFADE = REG_MAPSET = 0;
	REG_SCROLLX1 = REG_SCROLLX2 = REG_SCROLLY1 = REG_SCROLLY2 = 0;
	OBJECTS_A[0] = 0;
	interface->vBlank = 0;
	interface->drawCharFont = (char*)TEXTFONT_BOLD8;
	interface->drawCharHeight = 0x0808;
	interface->io.attribs = 0x0F;
	ClearScreen();
	ResetPalette();
}

bool AnyDisks()
{
	for (int i = 0; i < interface->io.numDrives; i++)
	{
		volatile uint8_t* firstDisk = (uint8_t*)MEM_DEVS + (interface->io.diskToDev[i] * 0x8000);
		if (firstDisk[4] & 1)
			return true;
	}
	return false;
}

void LoadConfig(int drive)
{
	FILE file;
	FILEINFO nfo;
	char cfgData[512];
	char path[16];
	strcpy(path, "a:/start.cfg");
	path[0] = 'a' + drive;
	//printf("checking %s...", path);
	if (OpenFile(&file, path, FA_READ)) return;
	ReadFile(&file, cfgData, 512);
	CloseFile(&file);

	char* ptr = cfgData;
	char* key;
	char* value;

	char fontName[16] = { 0 };
	char locName[16] = { 0 };

	while (*ptr != 0)
	{
		if (*ptr == 0xD) *ptr = 0xA;
		if (*ptr == '\n' || *ptr == ';')
		{
			while(*ptr != '\n') ptr++;
			ptr++;
			continue;
		}

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
		{
			strcpy(fontName, path);
			strcpy(fontName + 3, value);
		}
		else if (!strncmp(key, "locale", 8))
		{
			strcpy(locName, path);
			strcpy(locName + 3, value);
		}
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

__attribute__ ((noreturn, cold))
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
