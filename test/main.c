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

#define NUMOPTS 10
extern void TextTest();
extern void BitmapTest();
extern void KeyboardTest();
extern void MouseTest();

extern const uint16_t pointerTiles[];
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

void JoypadTest()
{
	MISC->SetTextMode(SMODE_320 | SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&pointerTiles, 0x2E0, DMA_INT);
	SPRITES_A[0] = SPRITEA_BUILD(256, 1, 0);
	SPRITES_B[0] = SPRITEB_BUILD(320, 160, 0, 0, 0, 0, 1, 0);
	while (1)
	{
		REG_JOYPAD = 1; //reset
		int dpadbuts = REG_JOYPAD & 0xFF; //int with mask to prevent "0xFFFFFF80"
		int extrabuts = REG_JOYPAD & 0xFF;
		char axis1 = REG_JOYPAD;
		char axis2 = REG_JOYPAD;

		TEXT->SetCursorPosition(0, 0);
		printf("0x%02X 0x%02X %3d,%3d  ", dpadbuts, extrabuts, axis1, axis2);

		SPRITES_B[0] = SPRITEB_BUILD(160 + (axis1 / 2), 120 + (axis2 / 2), 0, 0, 0, 0, 1, 0);

		vbl();
	}
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
};
const void* const optionFuncs[] =
{
	TextTest,
	BitmapTest,
	0,
	KeyboardTest,
	MouseTest,
	JoypadTest,
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
