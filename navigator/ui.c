#include "nav.h"

#define NUMMENUS 2

const tMenuItem fileMenu[] = {
	{ "Yapok", 0, 42 },
	{ "Yut-yut", 0, 43 },
	{ "Yamane", 1, 0  },
	{ "Yak", 0, 44 },
	{ "-", 1, 0 },
	{ "Bacon!", 0, 46 },
};

const tMenuItem testMenu[] = {
	{ "Yapok", 0, 42 },
	{ "Yut-yut", 0, 43 },
};

const tMenu menuBar[] =
{
	{ "File", 6, fileMenu },
	{ "Test", 2, testMenu },
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
	TEXT->SetTextColor(4, 15);
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
			TEXTMAP[o++] = (('1' + i) << 8) | 0x1B;
		else
		{
			TEXTMAP[o++] = ('1' << 8) | 0x1B;
			TEXTMAP[o++] = ('0' << 8) | 0x1B;
		}
		for (int j = 0; j < 6; j++)
		{
			TEXTMAP[o++] = (keys[i][j] << 8) | 0x1F;
		}
		TEXTMAP[o++] = 0x201F;
	}
}

void DrawMenu()
{
	short o = 0;
	char* c;
	for (int i = 0; i < NUMMENUS; i++)
	{
		menuLefts[i] = o;
		TEXTMAP[o++] = 0x201F;
		c = menuBar[i].title;
		while (*c)
			TEXTMAP[o++] = (*c++ << 8) | 0x1F;
		TEXTMAP[o++] = 0x201F;

		for (int j = 0; j < menuBar[i].numItems; j++)
		{
			int l = strlen(menuBar[i].items[j].title);
			if (l > menuWidths[i])
				menuWidths[i] = l;
		}
	}
	for (; o < 80;)
		TEXTMAP[o++] = 0x201F;
}

void Highlight(char left, char top, char width, unsigned char color)
{
	short o = (top * 80) + left;
	for (int i = 0; i < width; i++, o++)
	{
		TEXTMAP[o] = (TEXTMAP[o] & ~0x00FF) | color;
	}
}

void DropMenu(int c)
{
	if (menuWindow != NULL)
		CloseWindow(menuWindow);
	menuWindow = OpenWindow(menuLefts[c], 1, menuWidths[c] + 4, menuBar[c].numItems + 2, 0x1F);
	for (int i = 0; i < menuBar[c].numItems; i++)
	{
		if (menuBar[c].items[i].title[0] == '-')
		{
			short o = ((2 + i) * 80) + menuLefts[c];
			TEXTMAP[o++] = 0x8F1F; //|-
			for (int j = 0; j < menuWidths[c] + 2; j++)
				TEXTMAP[o++] = 0x901F; //--
			TEXTMAP[o] = 0x8A1F; //-|
			continue;
		}
		TEXT->SetTextColor(1, menuBar[c].items[i].state == 0 ? 15 : 7);
		TEXT->SetCursorPosition(menuLefts[c] + 2, 2 + i);
		TEXT->Write(menuBar[c].items[i].title);
	}
	Highlight(menuLefts[c] + 1, 2, menuWidths[c] + 2, 0x9F);
}

void OpenMenu()
{
	int cm = 0;
	int ci = 0;
	Highlight(menuLefts[cm], 0, strlen(menuBar[cm].title) + 2, 0x9F);
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
				Highlight(menuLefts[cm], 0, strlen(menuBar[cm].title) + 2, 0x1F);
				if (cm == 0) cm = NUMMENUS;
				cm--;
				ci = 0;
				Highlight(menuLefts[cm], 0, strlen(menuBar[cm].title) + 2, 0x9F);
				DropMenu(cm);
			}
			else if (key == 0xCD) //right
			{
				Highlight(menuLefts[cm], 0, strlen(menuBar[cm].title) + 2, 0x1F);
				cm++;
				ci = 0;
				if (cm == NUMMENUS) cm = 0;
				Highlight(menuLefts[cm], 0, strlen(menuBar[cm].title) + 2, 0x9F);
				DropMenu(cm);
			}
			else if (key == 0xC8) //up
			{
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 2, 0x1F);
				if (ci == 0) ci = menuBar[cm].numItems;
				ci--;
				if (menuBar[cm].items[ci].state & 1) ci--;
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 2, 0x9F);
			}
			else if (key == 0xD0) //down
			{
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 2, 0x1F);
				ci++;
				if (menuBar[cm].items[ci].state & 1) ci++;
				if (ci == menuBar[cm].numItems) ci = 0;
				Highlight(menuLefts[cm] + 1, 2 + ci, menuWidths[cm] + 2, 0x9F);
			}
		}
	}
}
