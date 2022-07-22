#include "../ass.h"
IBios* interface;

#define SRAM ((uint8_t*)(0x00FE0000))

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
	while (INP_KEYIN == 0)
	{
		TEXTMAP[(29 * 80) + 29] = 0x0E | (spinner[((spin++) >> 4) % 11] << 8);
		TEXTMAP[(29 * 80) + 28] = 0x0E | (spinner[((spin >> 4) + 1) % 11] << 8);
		TEXTMAP[(29 * 80) + 27] = 0x0E | (spinner[((spin >> 4) + 2) % 11] << 8);
		vbl();
	}
	for (int i = 0; i < 30; i++)
		TEXTMAP[(29 * 80) + i] = 0x2007;
}

char getchar()
{
	uint16_t key = 0;
	while (1)
	{
		vbl();
		key = INP_KEYIN;
		if ((key & 0xFF) > 0)
			break;
	}
	return sctoasc[key & 0xFF];
}

extern uint16_t tilegrid[];
extern uint8_t tilesTiles[];
void TilemapTest()
{
	MISC->SetTextMode(SMODE_TILE);
	MISC->DmaCopy(TILESET, (int8_t*)&tilesTiles, 256, DMA_INT);
	REG_MAPSET = 0x10;
	uint16_t* dst = MAP1;
	uint16_t* src = (uint16_t*)tilegrid;
	for (int line = 0; line < 30; line++)
	{
		for (int row = 0; row < 40; row++)
			*dst++ = *src++;
		dst += 24;
	}
	while (INP_KEYIN == 0) vbl();
}

#define NUMOPTS 11
extern void TextTest();
extern void GraphicsTest();
extern void KeyboardTest();
extern void MouseTest();
extern void JoypadTest();
extern void ClockTest();
extern void MIDITest();
extern void PCMTest();
extern void OPLTest();

const char* const optionNames[] =
{
	"Textmode",
	"Bitmap graphics",
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
	GraphicsTest,
	TilemapTest,
	KeyboardTest,
	MouseTest,
	JoypadTest,
	ClockTest,
	0,
	MIDITest,
	PCMTest,
	OPLTest
};

int main(void)
{
	int timesRan = SRAM[8];
	timesRan++;
	SRAM[8] = timesRan;
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

		TEXT->Write("Times ran: %d.\n", timesRan);

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
