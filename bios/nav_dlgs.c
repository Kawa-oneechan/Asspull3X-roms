#include "nav.h"

int MessageBox(const char* message, int type)
{
	int maxWidth = 0;
	char msg[strlen(message) + 10];
	int l = 0;
	const char* lines[10]; //let's assume this many lines total.

	{
		char* m1 = (char*)message;
		char* m2 = msg;
		lines[l++] = m2;
		int w = 0;
		while (true)
		{
			if (*m1 == '\n' || *m1 == '\0')
			{
				if (w > maxWidth)
				{
					maxWidth = w;
					w = 0;
					*m2++ = '\0';
					lines[l++] = m2;
				}
				if (*m1 == '\0')
				{
					l--;
					break;
				}
				m1++;
			}
			else
			{
				w++;
				*m2++ = *m1++;
			}
		}
	}

	int h = l + 2;
	int butts = 0;
	const char* buttons[3] = { 0 };
	char keys[6] = { 0 };

	{
		butts = 1;
		buttons[0] = "  OK   ";
		keys[0] = KEYSCAN_ENTER;
		keys[3] = KEYSCAN_O;
	}
	if (type == 1)
	{
		butts = 2;
		buttons[0] = "Cancel ";
		keys[0] = KEYSCAN_ESCAPE;
		keys[3] = KEYSCAN_C;
	}
	else if (type == 2)
	{
		butts = 2;
		buttons[0] = " Abort ";
		buttons[1] = " Retry ";
		keys[0] = KEYSCAN_ESCAPE;
		keys[1] = KEYSCAN_ENTER;
		keys[3] = KEYSCAN_A;
		keys[4] = KEYSCAN_R;
	}

	tWindow* win = OpenWindow(-1, -1, maxWidth + 8, 4 + h, CLR_DIALOG);
	SetTextColor(SplitColor(CLR_DIALOG));
	for (int i = 0; i < l; i++)
	{
		SetCursorPosition(win->left + 4, win->top + 2 + i);
		Write(lines[i]);
	}
	SetTextColor(SplitColor(0x8F));
	for (int i = 0; i < butts; i++)
	{
		SetCursorPosition(win->left + 4, win->top + 3 + l);
		Write(buttons[i]);
	}

	int ret = -1;
	while (ret < 0)
	{
		key = INP_KEYIN;
		for (int i = 0; i < butts; i++)
		{
			if (key == keys[i] || key == keys[i + 3])
			{
				ret = i;
				break;
			}
		}
	}

	CloseWindow(win);

	return ret;
}

int SwitchDrive(int which, int now)
{
	const uint16_t abcd[] = { 30, 48, 46, 32 };
	int numDrives = interface->io.numDrives;

	tWindow* win = OpenWindow((WIDTH >> 1) - 2 + (WIDTH * which), 8, 9, numDrives + 2, CLR_DIALOG);
	for (int i = 0; i < numDrives; i++)
	{
		//int16_t icon = ((0xB9 + (driveTypes[i] << 1)) << 8) | ((CLR_DIALOG >> 4) << 4);
		int16_t o = ((win->top + 1 + i) * 80) + win->left + 2;
		//TEXTMAP[o++] = icon;
		//TEXTMAP[o++] = icon + 0x100;
		o++;
		TEXTMAP[o++] = (('A' + i) << 8) | CLR_DIALOG;
		TEXTMAP[o++] = (':' << 8) | CLR_DIALOG;
	}
	int ret = now;
	Highlight(win->left + 1, win->top + 1 + ret, win->width - 4, 0x90);
	while (1)
	{
		key = INP_KEYIN;
		if ((key & 0xFF) > 0)
		{
			for (int i = 0; i < numDrives; i++)
			{
				if (key == abcd[i])
				{
					CloseWindow(win);
					return i;
				}
			}
			if (key == 0xC8) //up
			{
				Highlight(win->left + 1, win->top + 1 + ret, win->width - 4, CLR_DIALOG);
				if (ret == 0) ret = numDrives;
				ret--;
				Highlight(win->left + 1, win->top + 1 + ret, win->width - 4, 0x90);
			}
			else if (key == 0xD0) //down
			{
				Highlight(win->left + 1, win->top + 1 + ret, win->width - 4, CLR_DIALOG);
				ret++;
				if (ret == numDrives) ret = 0;
				Highlight(win->left + 1, win->top + 1 + ret, win->width - 4, 0x90);
			}
			else if (key == 0x1C) //enter
			{
				volatile uint8_t* firstDisk = (uint8_t*)0x02000000 + (interface->io.diskToDev[ret] * 0x8000);
				if (firstDisk[4] & 1)
				{
					CloseWindow(win);
					return ret;
				}
				else
				{
					MessageBox("No disk.", 0);
				}
			}
			else if (key == 0x01) //escape
			{
				CloseWindow(win);
				return now;
			}
		}
	}
}

int ChangeAttributes(char* filePath)
{
	FILEINFO info;
	FileStat(filePath, &info);
	const uint8_t attribs[] = { AM_READONLY, AM_HIDDEN, AM_SYSTEM, AM_ARCHIVE };
	const char* const names[] = { "Read-only", "Hidden", "System", "Archive" };
	tWindow* win = OpenWindow(-1, -1, 20, 7, CLR_DIALOG);

	//"The feline urge to add a disabled 'Directory' checkbox." -- Kawa, March 7, 2022
	SetTextColor(CLR_DIALOG >> 4, 8);
	SetCursorPosition(win->left + 2, win->top + 1);
	Write("%s Directory", (info.fattrib & AM_DIRECTORY) ? "[X]" : "[ ]");

	for (int i = 0; i < 4; i++)
	{
		SetTextColor(CLR_DIALOG >> 4, 0);
		SetCursorPosition(win->left + 2, win->top + 2 + i);
		if (info.fattrib & attribs[i])
			Write("[X] ");
		else
			Write("[ ] ");
		SetTextColor(SplitColor(CLR_DIALOG));
		Write(names[i]);
	}
	Highlight(win->left + 2,  win->top + 2, win->width - 6, 0x90);

	int tab = 0;
	while (1)
	{
		vbl();
		key = INP_KEYIN;
		if ((key & 0xFF) > 0)
		{
			if (key == KEYSCAN_UP)
			{
				Highlight(win->left + 2,  win->top + 2 + tab, win->width - 6, CLR_DIALOG);
				if (tab == 0) tab = 4;
				tab--;
				Highlight(win->left + 2,  win->top + 2 + tab, win->width - 6, 0x90);
			}
			else if (key == KEYSCAN_DOWN || key == KEYSCAN_TAB)
			{
				Highlight(win->left + 2,  win->top + 2 + tab, win->width - 6, CLR_DIALOG);
				tab++;
				if (tab == 4) tab = 0;
				Highlight(win->left + 2,  win->top + 2 + tab, win->width - 6, 0x90);
			}
			else if (key == KEYSCAN_SPACE)
			{
				info.fattrib ^= attribs[tab];
				SetTextColor(9, 0);
				SetCursorPosition(win->left + 2, win->top + 2 + tab);
				Write(info.fattrib & attribs[tab] ? "[X]" : "[ ]");
			}
			else if (key == KEYSCAN_ENTER || key == KEYSCAN_ESCAPE)
			{
				if (key == KEYSCAN_ENTER)
					FileAttrib(filePath, info.fattrib);
				CloseWindow(win);
				return key == KEYSCAN_ENTER;
			}
		}
	}
	return 0;
}
