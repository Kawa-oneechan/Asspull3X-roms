#include "nav.h"

tMenuItem leftMenu[] = {
	{ "~Files", 33, CHECKED, 101 },
	{ "~Info", 23, 0, 102 },
	{ "~Preview", 25, 1, 103  },
	{ "Director~y info", 21, DISABLED, 104 },
	{ "~On/off           Ctrl-F1", 24, DISABLED, 105 },
	{ "-", 0, 1, 0 },
	{ "~Drive\x98                F1", 32, 0, 106 },
};

const tMenuItem filesMenu[] = {
	{ "~View             F2", 47, 0, 11 },
	{ "~Edit             F3", 18, DISABLED, 12 },
	{ "~Copy             F5", 46, DISABLED, 13 },
	{ "~Rename or Move   F6", 19, DISABLED, 14 },
	{ "~Make directory   F7", 50, 0, 15 },
	{ "~Delete           F8", 32, DISABLED, 16 },
	{ "~Print            F9", 25, 0, 17 },
	{ "File ~attributes", 30, 0, 18 },
};

const tMenuItem commandsMenu[] = {
	{ "~Swap panels        Ctrl-U", 31, 0, 31 },
	{ "-", 0, 1, 0 },
	{ "~Copy diskette\x98", 46, DISABLED, 32 },
	{ "~Format diskette\x98", 33, DISABLED, 33 },
	{ "~Label disk\x98", 38, DISABLED, 34 },
	{ "-", 0, 1, 0 },
	{ "Confi~guration", 34, DISABLED, 35 },
};

tMenuItem rightMenu[] = {
	{ "~Files", 33, CHECKED, 111 },
	{ "~Info", 23, 0, 112 },
	{ "~Preview", 25, DISABLED, 113  },
	{ "Director~y info", 21, DISABLED, 114 },
	{ "~On/off           Ctrl-F2", 24, DISABLED, 115 },
	{ "-", 0, 1, 0 },
	{ "~Drive\x98                F2", 32, 0, 116 },
};

const tMenu menuBar[] =
{
	{ "~Left", 38, 7, leftMenu },
	{ "~Files", 33, 8, filesMenu },
	{ "~Commands", 46, 7, commandsMenu },
	{ "~Right", 19, 7, rightMenu },
};

char menuLefts[NUMMENUS], menuWidths[NUMMENUS];

tWindow* menuWindow = NULL;

void WaitForKey()
{
	while ((key = INP_KEYIN) == 0) { vbl(); }
}

bool GetString(char left, char top, size_t width, size_t max, uint8_t color, char* text)
{
	SetTextColor(SplitColor(color));
	SetCursorPosition(left, top);
	int len = strlen(text);
	Write(text);
	for (int i = len; i < max; i++)
		WriteChar(' ');

	while (true)
	{
		SetCursorPosition(left + len, top);
		REG_CARET |= 0x8000;

		while ((key = INP_KEYIN) == 0) vbl();

		char ascii = interface->locale.sctoasc[(INP_KEYSHIFT & 1) ? key + 128 : key];

		if (key == KEYSCAN_ENTER)
			return true;
		else if (key == KEYSCAN_ESCAPE)
			return false;
		else if (key == KEYSCAN_BACKSP && len > 0)
		{
			len--;
			text[len] = '\0';
			SetCursorPosition(left + len, top);
			WriteChar(' ');
		}
		else if (len < max && (unsigned int)ascii - 0x21 < 0x5E);
		{
			text[len] = ascii;
			SetCursorPosition(left + len, top);
			WriteChar(text[len]);
			len++;
		}
	}
}

void DrawWindow(char left, char top, char width, char height, uint8_t color, bool shadow)
{
	uint16_t c;
	if (shadow)
	{
		for (int i = 1; i < height; i++)
		{
			if (i + top < 0) continue;
			if (i + top >= 30) break;
			for (int j = 0; j < width; j++)
			{
				if (j + left < 0) continue;
				if (j + left >= 80) break;
				int o = ((i + top) * 80) + j + left;
				c = TEXTMAP[o];
				if (i == height - 1 && j > 1)
					c = (c & 0xFF00) | 0x08;
				else if (j > width - 3)
					c = (c & 0xFF00) | 0x08;
				TEXTMAP[o] = c;
			}
		}

		width -= 2;
		height -= 1;
	}
	for (int i = 0; i < height; i++)
	{
		if (i + top < 0) continue;
		if (i + top >= 30) break;
		for (int j = 0; j < width; j++)
		{
			if (j + left < 0) continue;
			if (j + left >= 80) break;
			int o = ((i + top) * 80) + j + left;
			if (i == 0 || i == height - 1)
			{
				c = 0x9000 | color; //top or bottom edge
				if (j == 0)
				{
					if (i == 0)
						c = 0x9300 | color; //top left
					else
						c = 0x8C00 | color; //bottom left
				}
				else if (j == width - 1)
				{
					if (i == 0)
						c = 0x8B00 | color; //top right
					else
						c = 0x9200 | color; //bottom right
				}
			}
			else
			{
				c = 0x2000 | color; //space
				if (i == height - 1)
				{
					c = TEXTMAP[o];
					if (j > 1)
						c = (c & 0xFF00) | 0x08;
				}
				else if (j == 0 || j == width - 1)
					c = 0x8900 | color; //sides
				else if (shadow && j > width - 1)
					c = (TEXTMAP[o] & 0xFF00) | 0x08;
			}
			TEXTMAP[o] = c;
		}
	}
}

tWindow* OpenWindow(char left, char top, char width, char height, uint8_t color)
{
	if (left == -1) left = 40 - (width >> 1);
	if (top == -1) top = 12 - (height >> 1);
	tWindow *win = (tWindow*)HeapAlloc(sizeof(tWindow));
	width += 2;
	height++;
	win->left = left;
	win->top = top;
	win->width = width;
	win->height = height;
	win->bits = (uint16_t*)HeapAlloc(sizeof(uint16_t) * (width * height));
	uint16_t* b = win->bits;
	for (int i = 0; i < height; i++)
	{
		if (i + top < 0) continue;
		if (i + top >= 30) break;
		for (int j = 0; j < width; j++)
		{
			if (j + left < 0) continue;
			if (j + left >= 80) break;
			int o = ((i + top) * 80) + j + left;
			*b++ = TEXTMAP[o];
		}
	}
	DrawWindow(left, top, width, height, color, true);
	return win;
}

void CloseWindow(tWindow* win)
{
	uint16_t* b = win->bits;
	for (int i = 0; i < win->height; i++)
	{
		if (i + win->top < 0) continue;
		if (i + win->top >= 30) break;
		for (int j = 0; j < win->width; j++)
		{
			if (j + win->left < 0) continue;
			if (j + win->left >= 80) break;
			int o = ((i + win->top) * 80) + j + win->left;
			TEXTMAP[o] = *b++;
		}
	}
	HeapFree(win->bits);
	HeapFree(win);
}

void DrawPanel(char left, char top, char width, char height, uint8_t color)
{
	DrawWindow(left, top, width, height, color, false);
}

void DrawKeys(const char** keys)
{
	int o = 29 * 80;
	for (int i = 0; i < 10; i++)
	{
		if (i < 9)
			TEXTMAP[o++] = (('1' + i) << 8) | CLR_KEYNUM;
		else
		{
			TEXTMAP[o++] = ('1' << 8) | CLR_KEYNUM;
			TEXTMAP[o++] = ('0' << 8) | CLR_KEYNUM;
		}
		for (int j = 0; j < 6; j++)
		{
			TEXTMAP[o++] = (keys[i][j] << 8) | CLR_KEYTEXT;
		}
		TEXTMAP[o++] = 0x2000 | CLR_KEYNUM;
	}
}

static size_t myStrLen(const char* str)
{
	size_t i;
	if (str == 0) return 0;
	for (i = 0; i < 32 && *str; i++, str++)
		if (*str == '~') i--;
	return i;
}

void DrawMenu()
{
	int o = 0;
	char* c;
	for (int i = 0; i < NUMMENUS; i++)
	{
		menuLefts[i] = o;
		TEXTMAP[o++] = 0x2000 | CLR_MENUBAR;
		TEXTMAP[o++] = 0x2000 | CLR_MENUBAR;
		c = menuBar[i].title;
		while (*c)
		{
			if (*c == '~')
			{
				c++;
				TEXTMAP[o++] = (*c++ << 8) | CLR_MENUBARKEY;
			}
			else
				TEXTMAP[o++] = (*c++ << 8) | CLR_MENUBAR;
		}
		TEXTMAP[o++] = 0x2000 | CLR_MENUBAR;
		TEXTMAP[o++] = 0x2000 | CLR_MENUBAR;

		for (int j = 0; j < menuBar[i].numItems; j++)
		{
			int l = myStrLen(menuBar[i].items[j].title);
			if (l > menuWidths[i])
				menuWidths[i] = l;
		}
	}
	for (; o < 80;)
		TEXTMAP[o++] = 0x2000 | CLR_MENUBAR;
}

void Highlight(char left, char top, char width, uint8_t color)
{
	int o = (top * 80) + left;
	for (int i = 0; i < width; i++, o++)
	{
		TEXTMAP[o] = (TEXTMAP[o] & ~0x00F0) | (color & 0xF0);
	}
}

void DropMenu(int c)
{
	if (menuWindow != NULL)
		CloseWindow(menuWindow);
	menuWindow = OpenWindow(menuLefts[c], 1, menuWidths[c] + 6, menuBar[c].numItems + 2, CLR_MENU);
	for (int i = 0; i < menuBar[c].numItems; i++)
	{
		if (menuBar[c].items[i].title[0] == '-')
		{
			int o = ((2 + i) * 80) + menuLefts[c];
			TEXTMAP[o++] = 0x8F00 | CLR_MENU; //|-
			for (int j = 0; j < menuWidths[c] + 4; j++)
				TEXTMAP[o++] = 0x9000 | CLR_MENU; //--
			TEXTMAP[o] = 0x8A00 | CLR_MENU; //-|
			continue;
		}
		int o = ((2 + i) * 80) + menuLefts[c] + 3;
		if (menuBar[c].items[i].state & 2)
			TEXTMAP[o - 2] = 0x1000 | CLR_MENUITEM;

		char *ch = menuBar[c].items[i].title;
		while (*ch)
		{
			if (*ch == '~')
			{
				ch++;
				TEXTMAP[o++] = (*ch++ << 8) | (menuBar[c].items[i].state & 1 ? CLR_MENUDIS : CLR_MENUITEMKEY);
			}
			else
				TEXTMAP[o++] = (*ch++ << 8) | (menuBar[c].items[i].state & 1 ? CLR_MENUDIS : CLR_MENUITEM);
		}
	}
	Highlight(menuLefts[c] + 1, 2, menuWidths[c] + 4, CLR_MENUSEL);
}

char OpenMenu(int cm)
{
	int ci = 0;
	Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUSEL);
	DropMenu(cm);
	while (1)
	{
		key = INP_KEYIN;
		if ((key & 0xFF) > 0)
		{
			if (key == KEYSCAN_LEFT)
			{
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUBAR);
				if (cm == 0) cm = NUMMENUS;
				cm--;
				ci = 0;
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUSEL);
				DropMenu(cm);
			}
			else if (key == KEYSCAN_RIGHT)
			{
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUBAR);
				cm++;
				ci = 0;
				if (cm == NUMMENUS) cm = 0;
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUSEL);
				DropMenu(cm);
			}
			else if (key == KEYSCAN_UP)
			{
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 4, CLR_MENUITEM);
				if (ci == 0) ci = menuBar[cm].numItems;
				ci--;
				while (menuBar[cm].items[ci].state & DISABLED) ci--;
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 4, CLR_MENUSEL);
			}
			else if (key == KEYSCAN_DOWN)
			{
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 4, CLR_MENUITEM);
				ci++;
				while (menuBar[cm].items[ci].state & DISABLED) ci++;
				if (ci == menuBar[cm].numItems) ci = 0;
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 4, CLR_MENUSEL);
			}
			else if (key == KEYSCAN_ENTER || key == KEYSCAN_ESCAPE)
			{
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUBAR);
				CloseWindow(menuWindow);
				menuWindow = NULL;
				return (key == KEYSCAN_ENTER) ? menuBar[cm].items[ci].code : 0;
			}
		}
	}
}
