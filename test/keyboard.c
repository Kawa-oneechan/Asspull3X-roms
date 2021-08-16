#include "../ass.h"
extern IBios* interface;

extern const char keyboardImage[];

typedef struct
{
	int8_t left, top;
	const char* text;
} key;

const key keys[] =
{
	//0
	{  0, 0, 0       },
	{  5, 2, "Esc"   },
	{ 10, 7, "1"     },
	{ 15, 7, "2"     },
	{ 20, 7, "3"     },
	{ 25, 7, "4"     },
	{ 30, 7, "5"     },
	{ 35, 7, "6"     },
	{ 40, 7, "7"     },
	{ 45, 7, "8"     },
	//10
	{ 50, 7, "9"     },
	{ 55, 7, "0"     },
	{ 60, 7, "-"     },
	{ 65, 7, "="     },
	{ 70, 7, "BkSpc" },
	{  5,10, "Tab"   },
	{ 11,10, "Q"     },
	{ 16,10, "W"     },
	{ 21,10, "E"     },
	{ 26,10, "R"     },
	//20
	{ 31,10, "T"     },
	{ 36,10, "Y"     },
	{ 41,10, "U"     },
	{ 46,10, "I"     },
	{ 51,10, "O"     },
	{ 56,10, "P"     },
	{ 61,10, "["     },
	{ 66,10, "]"     },
	{ 67,13, "Return"},
	{  5,13, "Caps"  }, //unsure, I have my Windows set to use Caps like Shift
	//30
	{ 12,13, "A"     },
	{ 17,13, "S"     },
	{ 22,13, "D"     },
	{ 27,13, "F"     },
	{ 32,13, "G"     },
	{ 37,13, "H"     },
	{ 42,13, "J"     },
	{ 47,13, "K"     },
	{ 52,13, "L"     },
	{ 57,13, ";"     },
	//40
	{ 62,13, "'"     },
	{  5, 7, "`"     },
	{  0, 0, 0       },
	{ 71,10, "\\"    },
	{ 16,16, "Z"     },
	{ 21,16, "X"     },
	{ 26,16, "C"     },
	{ 31,16, "V"     },
	{ 36,16, "B"     },
	{ 41,16, "N"     },
	//50
	{ 46,16, "M"     },
	{ 51,16, "<"     },
	{ 56,16, ">"     },
	{ 61,16, "/"     },
	{  5,16, "Shift" }, //really 256
	{ 17,19, "Alt"   }, //really 512
	{  5,19, "Ctrl"  }, //really 1024
	{ 36,19, "Space" },
	{  0, 0, 0       },
	{ 15, 2, "F1"    },
	//60
	{ 20, 2, "F2"    },
	{ 25, 2, "F3"    },
	{ 30, 2, "F4"    },
	{ 36, 2, "F5"    },
	{ 41, 2, "F6"    },
	{ 46, 2, "F7"    },
	{ 51, 2, "F8"    },
	{ 57, 2, "F9"    },
	{ 62, 2, "F10"   },
	{ 67, 2, "F11"   },
	//70
	{ 72, 2, "F12"   },
	{  0, 0, 0       },
	{  0, 0, 0       },
	{  0, 0, 0       },
	{  0, 0, 0       },
	{  0, 0, 0       },
	{  0, 0, 0       },
	{  0, 0, 0       },
	{  0, 0, 0       },
	{  0, 0, 0       },
	//80 -- remapping from 199
	{ 10,23, "Hm"    },
	{ 67,23, "\x18"  }, //up
	{ 15,23, "PUp"   },
	{ 21,23, "Pau"   }, //actually 69
	{ 62,26, "\x16"  }, //left
	{  0, 0, 0       },
	{ 72,26, "\x17"  }, //right
	{  0, 0, 0       },
	{ 10,26, "End"   },
	{ 67,26, "\x19"  }, //down
	//90
	{ 15,26, "PDn"   },
	{  5,23, "Ins"   },
	{  5,26, "Del"   },
};

void KeyboardTest()
{
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	MISC->DmaCopy(MAP1, (int8_t*)&keyboardImage, 80 * 30, DMA_INT);
	TEXT->SetTextColor(0, 7);
	TEXT->SetCursorPosition(38, 26);
	printf("Keyboard test 1.0");
	TEXT->SetCursorPosition(27, 27);
	printf("Press ESC five times to exit.");
	REG_CARET = 0;

	TEXT->SetTextColor(7, 0);
	for (int i = 1; i < 93; i++)
	{
		if (keys[i].text == 0)
			continue;
		TEXT->SetCursorPosition(keys[i].left, keys[i].top);
		printf(keys[i].text);
	}
	TEXT->SetCursorPosition(66, 16);
	printf("Shift");
	TEXT->SetCursorPosition(59, 19);
	printf("Alt");
	TEXT->SetCursorPosition(71, 19);
	printf("Ctrl");

	int escapes = 0;

	while (escapes < 5)
	{
		while (REG_KEYIN != 0) { vbl(); }
		while (REG_KEYIN == 0) { vbl(); }
		int theKey = REG_KEYIN;
		if (theKey == 69) theKey = 83;
		else if (theKey == 87) theKey = 69;
		else if (theKey == 88) theKey = 70;
		else if (theKey == 256) theKey = 54;
		else if (theKey == 512) theKey = 55;
		else if (theKey == 1024) theKey = 56;
		else if (theKey >= 199) theKey -= 119; //convert to 80

		int l = keys[theKey].left;
		int t = keys[theKey].top;
		char* text = (char*)keys[theKey].text;

		//TEXT->SetCursorPosition(0,0);
		//printf("theKey: %d   ", theKey);

		//highlight
		{
			TEXT->SetCursorPosition(l, t);
			TEXT->SetTextColor(7, 12);
			printf(text);
		}

		//hack to highlight both sides
		if (theKey == 54)
		{
			TEXT->SetCursorPosition(66, t);
			printf(text);
		}
		else if (theKey == 55)
		{
			TEXT->SetCursorPosition(59, t);
			printf(text);
		}
		else if (theKey == 56)
		{
			TEXT->SetCursorPosition(71, t);
			printf(text);
		}

		while (REG_KEYIN != 0) { vbl(); }

		//lowlight
		{
			TEXT->SetCursorPosition(l, t);
			TEXT->SetTextColor(7, 15);
			printf(text);
		}

		//hack to lowlight both sides
		if (theKey == 54)
		{
			TEXT->SetCursorPosition(66, t);
			printf(text);
		}
		else if (theKey == 55)
		{
			TEXT->SetCursorPosition(59, t);
			printf(text);
		}
		else if (theKey == 56)
		{
			TEXT->SetCursorPosition(71, t);
			printf(text);
		}

		if (theKey == 1)
		{
			escapes++;
			if (escapes == 4)
			{
				TEXT->SetCursorPosition(27, 27);
				TEXT->SetTextColor(0, 7);
				printf("Press ESC once more to exit. ");
			}
		}
	}
}
