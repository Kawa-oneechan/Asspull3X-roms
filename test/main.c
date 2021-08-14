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
	TEXT->SetCursorPosition(0, 28);
	printf("Press any key to continue.");
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
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

#define NUMOPTS 9
extern void TextTest();
extern void BitmapTest();
extern void KeyboardTest();
extern void MouseTest();

const char* const optionNames[] =
{
	"Textmode",
	"Bitmaps",
	"Tilemaps",
	"Keyboard",
	"Mouse",
	"Real-time clock",
	"Disk I/O",
	"MIDI out",
	"PCM out",
};
const void* const optionFuncs[] =
{
	TextTest,
	BitmapTest,
	0,
	KeyboardTest,
	MouseTest,
	0,
	0,
	0,
	0,
};

int main(void)
{
	interface = (IBios*)(0x01000000);

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
