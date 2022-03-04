#include "nav.h"

#define NUMMENUS 4

const tMenuItem leftMenu[] = {
	{ "~Files", 33, 2, 101 },
	{ "~Info", 23, 1, 102 },
	{ "~Preview", 25, 1, 103  },
	{ "Director~y info", 21, 1, 104 },
	{ "~On/off           Ctrl-F1", 24, 1, 105 },
	{ "-", 0, 1, 0 },
	{ "~Drive\x98                F1", 32, 0, 106 },
};

const tMenuItem filesMenu[] = {
	{ "~View             F2", 47, 0, 11 },
	{ "~Edit             F3", 18, 0, 12 },
	{ "~Copy             F5", 46, 0, 13 },
	{ "~Rename or Move   F6", 19, 0, 14 },
	{ "~Make directory   F7", 50, 0, 15 },
	{ "~Delete           F8", 32, 0, 16 },
	{ "~Print            F9", 25, 0, 17 },
	{ "File ~attributes", 30, 0, 18 },
};

const tMenuItem commandsMenu[] = {
	{ "~Swap panels        Ctrl-U", 31, 0, 31 },
	{ "-", 0, 1, 0 },
	{ "~Copy diskette\x98", 46, 0, 32 },
	{ "~Format diskette\x98", 33, 0, 33 },
	{ "~Label disk\x98", 38, 0, 34 },
	{ "-", 0, 1, 0 },
	{ "Confi~guration", 34, 0, 35 },
};

const tMenuItem rightMenu[] = {
	{ "~Files", 33, 2, 201 },
	{ "~Info", 23, 1, 202 },
	{ "~Preview", 25, 1, 203  },
	{ "Director~y info", 21, 1, 204 },
	{ "~On/off           Ctrl-F2", 24, 1, 205 },
	{ "-", 0, 1, 0 },
	{ "~Drive\x98                F2", 32, 0, 206 },
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
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

tWindow* OpenWindow(int left, int top, int width, int height, int color)
{
	if (left == -1) left = 40 - (width >> 1);
	if (top == -1) top = 12 - (height >> 1);
	tWindow *win = (tWindow*)malloc(sizeof(tWindow));
	width += 2;
	height++;
	win->left = left;
	win->top = top;
	win->width = width;
	win->height = height;
	win->bits = (unsigned short*)malloc(sizeof(unsigned short) * (width * height));
	unsigned short* b = win->bits;
	unsigned short c;
	for (int i = 0; i < height; i++)
	{
		if (i + top < 0) continue;
		if (i + top >= 30) break;
		for (int j = 0; j < width; j++)
		{
			if (j + left < 0) continue;
			if (j + left >= 80) break;
			short o = ((i + top) * 80) + j + left;
			*b++ = TEXTMAP[o];
			if (i == 0 || i == height - 2)
			{
				c = 0x9000 | color; //top or bottom edge
				if (j == 0)
				{
					if (i == 0)
						c = 0x9300 | color; //top left
					else
						c = 0x8C00 | color; //bottom left
				}
				else if (j == width - 3)
				{
					if (i == 0)
						c = 0x8B00 | color; //top right
					else
						c = 0x9200 | color; //bottom right
				}
				else if (j > width - 3)
				{
					c = TEXTMAP[o];
					if (i)
						c = (c & 0xFF00) | 0x08;
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
				else if (j == 0 || j == width - 3)
					c = 0x8900 | color; //sides
				else if (j > width - 3)
					c = (TEXTMAP[o] & 0xFF00) | 0x08;
			}
			TEXTMAP[o] = c;
		}
	}
	return win;
}

void CloseWindow(tWindow* win)
{
	unsigned short* b = win->bits;
	for (int i = 0; i < win->height; i++)
	{
		if (i + win->top < 0) continue;
		if (i + win->top >= 30) break;
		for (int j = 0; j < win->width; j++)
		{
			if (j + win->left < 0) continue;
			if (j + win->left >= 80) break;
			short o = ((i + win->top) * 80) + j + win->left;
			TEXTMAP[o] = *b++;
		}
	}
	free(win->bits);
	free(win);
}

void ShowError(const char* message)
{
	tWindow* win = OpenWindow(-1, -1, strlen((char*)message) + 8, 5, 0x4F);
	TEXT->SetTextColor(SplitColor(0x4F));
	TEXT->SetCursorPosition(win->left + 4, win->top + 2);
	printf(message);
	WaitForKey();
	CloseWindow(win);
}

void DrawPanel(int left, int top, int width, int height, int color)
{
	unsigned short c;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			short o = ((i + top) * 80) + j + left;
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
				if (j == 0 || j == width - 1)
					c = 0x8900 | color; //sides
			}
			TEXTMAP[o] = c;
		}
	}
}

void DrawKeys(const char** keys)
{
	short o = 29 * 80;
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

size_t myStrLen(const char* str)
{
	size_t i;
	if (str == 0) return 0;
	for (i = 0; i < 32 && *str; i++, str++)
		if (*str == '~') i--;
	return i;
}

void DrawMenu()
{
	short o = 0;
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

void Highlight(char left, char top, char width, unsigned char color)
{
	short o = (top * 80) + left;
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
			short o = ((2 + i) * 80) + menuLefts[c];
			TEXTMAP[o++] = 0x8F00 | CLR_MENU; //|-
			for (int j = 0; j < menuWidths[c] + 4; j++)
				TEXTMAP[o++] = 0x9000 | CLR_MENU; //--
			TEXTMAP[o] = 0x8A00 | CLR_MENU; //-|
			continue;
		}
		short o = ((2 + i) * 80) + menuLefts[c] + 3;
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

void OpenMenu()
{
	int cm = 0;
	int ci = 0;
	Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUSEL);
	DropMenu(cm);
	while (1)
	{
		unsigned short key = REG_KEYIN;
		intoff();
		if ((key & 0xFF) > 0)
		{
			while(1) { if (REG_KEYIN == 0) break; }

			if (key == 0xCB) //left
			{
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUBAR);
				if (cm == 0) cm = NUMMENUS;
				cm--;
				ci = 0;
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUSEL);
				DropMenu(cm);
			}
			else if (key == 0xCD) //right
			{
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUBAR);
				cm++;
				ci = 0;
				if (cm == NUMMENUS) cm = 0;
				Highlight(menuLefts[cm], 0, myStrLen(menuBar[cm].title) + 4, CLR_MENUSEL);
				DropMenu(cm);
			}
			else if (key == 0xC8) //up
			{
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 4, CLR_MENUITEM);
				if (ci == 0) ci = menuBar[cm].numItems;
				ci--;
				while (menuBar[cm].items[ci].state & 1) ci--;
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 4, CLR_MENUSEL);
			}
			else if (key == 0xD0) //down
			{
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 4, CLR_MENUITEM);
				ci++;
				while (menuBar[cm].items[ci].state & 1) ci++;
				if (ci == menuBar[cm].numItems) ci = 0;
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 4, CLR_MENUSEL);
			}
		}
	}
}
