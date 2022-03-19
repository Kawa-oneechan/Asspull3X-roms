#include "nav.h"

int ChangeAttributes(char* filePath)
{
	FILEINFO info;
	DISK->FileStat(filePath, &info);
	const uint8_t attribs[] = { AM_READONLY, AM_HIDDEN, AM_SYSTEM, AM_ARCHIVE };
	const char* const names[] = { "Read-only", "Hidden", "System", "Archive" };
	tWindow* win = OpenWindow(-1, -1, 20, 7, CLR_DIALOG);

	//"The feline urge to add a disabled 'Directory' checkbox." -- Kawa, March 7, 2022
	TEXT->SetTextColor(CLR_DIALOG >> 4, 8);
	TEXT->SetCursorPosition(win->left + 2, win->top + 1);
	printf("%s Directory", (info.fattrib & AM_DIRECTORY) ? "\xAC\xAD" : "\xAA\xAB");

	for (int i = 0; i < 4; i++)
	{
		TEXT->SetTextColor(CLR_DIALOG >> 4, 0);
		TEXT->SetCursorPosition(win->left + 2, win->top + 2 + i);
		if (info.fattrib & attribs[i])
			printf("\xAC\xAD ");
		else
			printf("\xAA\xAB ");
		TEXT->SetTextColor(SplitColor(CLR_DIALOG));
		printf(names[i]);
	}
	Highlight(win->left + 2,  win->top + 2, win->width - 6, 0x90);

	int tab = 0;
	while (1)
	{
		vbl();
		uint16_t key = INP_KEYIN;
		intoff();
		if ((key & 0xFF) > 0)
		{
			if (key == 0xC8) //up
			{
				Highlight(win->left + 2,  win->top + 2 + tab, win->width - 6, CLR_DIALOG);
				if (tab == 0) tab = 4;
				tab--;
				Highlight(win->left + 2,  win->top + 2 + tab, win->width - 6, 0x90);
			}
			else if (key == 0xD0 || key == 0x0F) //down or tab
			{
				Highlight(win->left + 2,  win->top + 2 + tab, win->width - 6, CLR_DIALOG);
				tab++;
				if (tab == 4) tab = 0;
				Highlight(win->left + 2,  win->top + 2 + tab, win->width - 6, 0x90);
			}
			else if (key == 0x39) //space
			{
				info.fattrib ^= attribs[tab];
				TEXT->SetTextColor(9, 0);
				TEXT->SetCursorPosition(win->left + 2, win->top + 2 + tab);
				printf(info.fattrib & attribs[tab] ? "\xAC\xAD" : "\xAA\xAB");
			}
			else if (key == 0x1C || key == 0x01) //enter or escape
			{
				if (key == 0x1C)
					DISK->FileAttrib(filePath, info.fattrib);
				CloseWindow(win);
				return key == 0x1C;
			}
		}
	}
	return 0;
}
