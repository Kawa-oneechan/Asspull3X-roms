#include "nav.h"

//TODO: replace with MessageBox?
void ShowError(const char* message)
{
	tWindow* win = OpenWindow(-1, -1, strlen((char*)message) + 8, 5, 0x4F);
	SetTextColor(SplitColor(0x4F));
	SetCursorPosition(win->left + 4, win->top + 2);
	Write(message);
	WaitForKey();
	CloseWindow(win);
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
		uint16_t key = INP_KEYIN;
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
				CloseWindow(win);
				return ret;
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
		uint16_t key = INP_KEYIN;
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
