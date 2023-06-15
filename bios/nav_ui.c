#include "nav.h"

typedef struct
{
	char* label;
	uint8_t type;
	uint8_t scan;
	uint8_t left, top;
	uint8_t width, height;
	uint8_t state;
	uint8_t id;
} tDialogItem;

typedef struct
{
	char* title;
	uint8_t width, height;
	uint8_t numItems;
	tDialogItem* items;
} tDialog;

tDialogItem testDialogItems[] = {
	{ "This is a test.", 0, 0, 1, 0, 16, 1, 0, 0 },
	{ " Button1 ", 1, KEYSCAN_B, 2, 2, 9, 1, 0, 100 },
	{ " Button2 ", 1, KEYSCAN_C, 12, 2, 9, 1, 0, 101 },
};
tDialog testDialog = {
	"Test", 22, 3, 3, testDialogItems
};

int DialogTest()
{
	tDialog* dlg = &testDialog;
	REG_SCREENFADE = 0;
	tWindow* win = OpenWindow(-1, -1, dlg->width + 2, dlg->height + 2, CLR_DIALOG);
	size_t len = strlen(dlg->title);
	SetTextColor(SplitColor(CLR_DIALOG));
	SetCursorPosition(win->left + (win->width / 2) - (len / 2), win->top);
	Write(dlg->title);

	int focus = 0;
	for (int i = 0; i < dlg->numItems; i++)
	{
		if (dlg->items[i].type != 0) //not a label, not disabled
		{
			focus = i;
			break;
		}
	}

	while(true)
	{
		for (int i = 0; i < dlg->numItems; i++)
		{
			tDialogItem* item =  &dlg->items[i];
			SetCursorPosition(win->left + 1 + item->left, win->top + 1 + item->top);
			switch(item->type)
			{
				case 0: //label
				{
					SetTextColor(SplitColor(CLR_DIALOG));
					Write(item->label);
					break;
				}
				case 1: //button
				{
					SetTextColor(8, (focus == i) ? 15 : 7);
					Write(item->label);
					break;
				}
			}
		}
		SetCursorPosition(0, 0);
		SetTextColor(0, 7);
		Write("focus: %d \n", focus);
		Write("id: %d \n", dlg->items[focus].id);
		while ((key = INP_KEYIN) == 0) vbl();
		if (key == KEYSCAN_TAB)
		{
			int tries = 2;
			focus++;
			while (tries && dlg->items[focus].type == 0) //not a label, not disabled
			{
				focus++;
				if (focus >= dlg->numItems)
				{
					focus = 0;
					tries--;
				}
			}
		}
		else if (key == KEYSCAN_ENTER)
		{
			break;
		}
		else
		{
			for (int i = 0; i < dlg->numItems; i++)
			{
				tDialogItem* item = &dlg->items[i];
				if (item->scan && item->scan == key)
				{
					focus = i;
					if (item->type == 1) //button
					{
						CloseWindow(win);
						return item->id;
					}
				}
			}
		}
	}
	CloseWindow(win);
	return dlg->items[focus].id;
}

void WaitForKey()
{
	while ((key = INP_KEYIN) == 0) { vbl(); }
}

bool GetString(char left, char top, size_t width, size_t max, uint8_t color, char* text)
{
	SetTextColor(SplitColor(color));
	SetCursorPosition(left, top);
	uint8_t len = strlen(text);
	Write(text);
	for (uint16_t i = len; i < max; i++)
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
		else if (len < max && (unsigned int)ascii - 0x21 < 0x5E)
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

void DrawPanelSeparator(char left, char top, char width, uint8_t color)
{
	TEXTMAP[(top * 80) + left] = 0x8F00 | color; //|-
	for (int i = 1; i < width - 1; i++)
		TEXTMAP[(top * 80) + left + i] = 0x9000 | color; //--
	TEXTMAP[(top * 80) + left + width - 1] = 0x8A00 | color; //-|
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

void Highlight(char left, char top, char width, uint8_t color)
{
	int o = (top * 80) + left;
	for (int i = 0; i < width; i++, o++)
	{
		TEXTMAP[o] = (TEXTMAP[o] & ~0x00F0) | (color & 0xF0);
	}
}
