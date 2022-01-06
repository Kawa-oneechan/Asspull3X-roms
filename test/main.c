#include "../ass.h"
IBios* interface;

static const char sctoasc[256] = {
	0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
	'q','w','e','r','t','y','u','i','o','p','[',']',0,0,'a','s',
	'd','f','g','h','j','k','l',';',39,'`',0,92,'z','x','c','v',
	'b','n','m',',','.','/',0,'*',0,32,0,0,0,0,0,0,
	0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
	'2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

void WaitForKey()
{
	//TEXT->SetCursorPosition(0, 28);
	//printf("Press any key to continue.");
	const char prompt[] = "Press any key to continue.";
	const char spinner[] = "\xB8\x08+*+\x08\xB8    ";
	int spin = 0;
	for (int i = 0; i < 26; i++)
		TEXTMAP[(29 * 80) + i] = 0x07 | (prompt[i] << 8);
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0)
	{
		TEXTMAP[(29 * 80) + 29] = 0x0E | (spinner[((spin++) >> 4) % 11] << 8);
		TEXTMAP[(29 * 80) + 28] = 0x0E | (spinner[((spin >> 4) + 1) % 11] << 8);
		TEXTMAP[(29 * 80) + 27] = 0x0E | (spinner[((spin >> 4) + 2) % 11] << 8);
		vbl();
	}
	while (REG_KEYIN != 0) { vbl(); }
	for (int i = 0; i < 30; i++)
		TEXTMAP[(29 * 80) + i] = 0x2007;
}

char getchar()
{
	unsigned short key = 0;
	while (1)
	{
		key = REG_KEYIN;
		if ((key & 0xFF) > 0)
			break;
	}
	while (1)
	{
		if ((REG_KEYIN & 0xFF) == 0)
			break;
	}
	return sctoasc[key & 0xFF];
}

#define NUMOPTS 11
extern void TextTest();
extern void BitmapTest();
extern void KeyboardTest();
extern void MouseTest();
extern void ClockTest();

extern const uint16_t pointerTiles[];
#define OBJECTA_BUILD(t,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((t) & 0x1FF) << 0)		\
)
#define OBJECTB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
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

void JoypadTest()
{
	MISC->SetTextMode(SMODE_320 | SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&pointerTiles, 0x2E0, DMA_INT);
	OBJECTS_A[0] = OBJECTA_BUILD(256, 1, 0);
	OBJECTS_B[0] = OBJECTB_BUILD(320, 160, 0, 0, 0, 0, 1, 0);
	while (1)
	{
		REG_JOYPAD = 1; //reset
		int dpadbuts = REG_JOYPAD & 0xFF; //int with mask to prevent "0xFFFFFF80"
		int extrabuts = REG_JOYPAD & 0xFF;
		char axis1 = REG_JOYPAD;
		char axis2 = REG_JOYPAD;

		TEXT->SetCursorPosition(0, 0);
		printf("0x%02X 0x%02X %3d,%3d  ", dpadbuts, extrabuts, axis1, axis2);

		OBJECTS_B[0] = OBJECTB_BUILD(160 + (axis1 / 2), 120 + (axis2 / 2), 0, 0, 0, 0, 1, 0);

		vbl();
	}
}

extern const unsigned char soundLoop[], soundOne[];

void PCMTest()
{
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	TEXT->Write("PCM audio test");

	TEXT->SetCursorPosition(0, 2);
	TEXT->Write("Playing a sample on loop.");
	REG_PCMOFFSET = (unsigned int)soundLoop + 4;
	REG_PCMLENGTH = *(unsigned int*)soundLoop | PCM_REPEAT;
	WaitForKey();
	REG_PCMLENGTH = 0;
	TEXT->SetCursorPosition(0, 3);
	TEXT->Write("The loop should have stopped.");
	WaitForKey();
	TEXT->SetCursorPosition(0, 4);
	TEXT->Write("Playing a sample once.");
	REG_PCMOFFSET = (unsigned int)soundOne + 4;
	REG_PCMLENGTH = *(unsigned int*)soundOne;
	WaitForKey();
}

extern void IMF_Play();
extern int IMF_LoadSong(const unsigned short *sauce, int loop);
extern const unsigned short imfData[];

void ResetOPL()
{
	const char regs[] = {
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35,
		0xA0, 0xA1, 0xA2, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
		0xB0, 0xB1, 0xB2, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xBD,
		0xC0, 0xC1, 0xC2, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8,
		0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
		0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5,
		0x08, 0x01,
	};
	for (int i = 0; i < 77; i++) REG_OPLOUT = regs[i] << 8;
}

void OPLTest()
{
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	TEXT->Write("OPL3 music test");

	TEXT->SetCursorPosition(0, 2);
	//TEXT->Write("Playing a sample on loop.");
	interface->VBlank = IMF_Play;
	IMF_LoadSong(imfData, 1);
	WaitForKey();
	interface->VBlank = 0;

	ResetOPL();
}

const char* const optionNames[] =
{
	"Textmode",
	"Bitmaps",
	"Tilemaps",
	"Keyboard",
	"Mouse",
	"Joypad",
	"Real-time clock",
	"Disk I/O",
	"MIDI out",
	"PCM out",
	"OPL out",
};
const void* const optionFuncs[] =
{
	TextTest,
	BitmapTest,
	0,
	KeyboardTest,
	MouseTest,
	JoypadTest,
	ClockTest,
	0,
	0, //MIDITest
	PCMTest,
	OPLTest
};

int main(void)
{
	while (1)
	{
		DRAW->ResetPalette();
		MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
		TEXT->SetTextColor(0, 7);
		TEXT->ClearScreen();

		TEXT->SetTextColor(0, 7);
		TEXT->Write("Welcome to the ");
		TEXT->SetTextColor(0, 1);
		TEXT->Write("Asspull ");
		TEXT->SetTextColor(0, 9);
		TEXT->WriteChar('\x96');
		TEXT->SetTextColor(0, 12);
		TEXT->WriteChar('\xD7');
		TEXT->SetTextColor(0, 7);
		TEXT->Write(" testing suite.\n");

		for (int i = 0; i < NUMOPTS; i++)
		{
			TEXT->SetCursorPosition(2, 4 + i);
			TEXT->SetTextColor(0, 11);
			TEXT->WriteChar('A' + i);
			TEXT->SetTextColor(0, (optionFuncs[i] == 0) ? 8 : 7);
			TEXT->Write(". ");
			TEXT->Write(optionNames[i]);
		}
		char option = getchar();
		if (option >= 'a' && option <= 'a' + NUMOPTS)
		{
			void (*func)(void) = (void*)optionFuncs[option-'a'];
			if (func == 0)
				continue;
			func();
		}
	}
}
