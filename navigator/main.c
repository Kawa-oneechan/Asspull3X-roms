#include "nav.h"

IBios* interface;

#define MAXPATH 512
#define MAXFILES 512

#define WIDTH 39
#define HEIGHT 24
#define FILESSHOWN (HEIGHT-2)

void PrintComma(long n)
{
    long n2 = 0;
    int scale = 1;
    if (n < 0)
    {
        printf("-");
        n = -n;
    }
    while (n >= 1000) {
        n2 = n2 + scale * (n % 1000);
        n /= 1000;
        scale *= 1000;
    }
    printf("%d", n);
    while (scale != 1)
    {
        scale /= 1000;
        n = n2 / scale;
        n2 = n2  % scale;
        printf(",%03d", n);
    }
}

char* filenames[2] = { 0 };
int fileCt[2] = { 0 };

void Populate(const char* path, int side, const char* pattern)
{
	int ret;
	DIR dir;
	FILEINFO info;
	fileCt[side] = 0;
	char* curFN = 0;

	intoff();

	if (filenames[side] == 0)
	{
		filenames[side] = (char*)malloc(MAXFILES * 16);
	}
	curFN = filenames[side];

tryOpenDir:
	ret = DISK->OpenDir(&dir, path);
	if (ret)
	{
		tWindow* error = OpenWindow(-1, -1, 50, 6, 0x1F);
		TEXT->SetTextColor(1, 15);
		TEXT->SetCursorPosition(error->left + 2, error->top + 1);
		printf("Disk error reading %s:", path);
		TEXT->SetTextColor(1, 9);
		TEXT->SetCursorPosition(error->left + 4, error->top + 2);
		if (ret == 3)
			printf("No disk inserted?");
		else
			printf(DISK->FileErrStr(ret));
		TEXT->SetTextColor(1, 14);
		TEXT->SetCursorPosition(error->left + 2, error->top + 4);
		printf("Abort or Retry? >");
		while (1)
		{
			char k = getchar();
			CloseWindow(error);
			if (k == 'a')
			{
				//Ask for another drive instead?
				strcpy_s(curFN, MAXPATH, "<ERROR>");
				fileCt[side]++;
				return;
			}
			else if (k == 'r')
				goto tryOpenDir;
		}
	}

	if (strnlen_s(path, MAXPATH) > 3)
	{
		strcpy_s(curFN, MAXPATH, "..");
		curFN += 16;
		fileCt[side]++;
	}

	while (fileCt[side] < MAXFILES)
	{
		ret = DISK->ReadDir(&dir, &info);
		if (ret != 0 || info.fname[0] == 0) break;
		if (info.fattrib & AM_DIRECTORY && !(info.fattrib & AM_HIDDEN))
		{
			strcpy_s(curFN, 13, info.fname);
			curFN += 16;
			fileCt[side]++;
		}
	}

	ret = DISK->FindFirst(&dir, &info, path, pattern);
	while(ret == 0 && info.fname[0] && fileCt[side] < MAXFILES)
	{
		if (!(info.fattrib & AM_HIDDEN))
		{
			strcpy_s(curFN, 13, info.fname);
			curFN += 16;
			fileCt[side]++;
		}
		ret = DISK->FindNext(&dir, &info);
	}
}

int SwitchDrive(int which, int now)
{
	const unsigned short abcd[] = { 30, 48, 46, 32 };
	int numDrives = DISK->GetNumDrives();
	tWindow* win = OpenWindow((WIDTH >> 1) - 2 + (WIDTH * which), 8, 8, numDrives + 2, CLR_DIALOG);
	for (int i = 0; i < numDrives; i++)
	{
		short o = ((win->top + 1 + i) * 80) + win->left + 2;
		TEXTMAP[o++] = (('A' + i) << 8) | CLR_DIALOG;
		TEXTMAP[o++] = (':' << 8) | CLR_DIALOG;
	}
	int ret = now;
	Highlight(win->left + 1, win->top + 1 + ret, win->width - 4, 0x90);
	while (1)
	{
		unsigned short key = REG_KEYIN;
		intoff();
		if ((key & 0xFF) > 0)
		{
			while(1) { if (REG_KEYIN == 0) break; }
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

void SelectFile(const char* path1, const char* path2, const char* pattern, char* selection, int(*onSelect)(char*))
{
	//Note: these are PLACEHOLDERS
	static const char* keys[] = {
		"Left  ",
		"Right ",
		"View  ",
		"Edit  ",
		"Copy  ",
		"RenMov",
		"Mkdir ",
		"Delete",
		"Print ",
		"PullDn",
	};

	int index[2] = { 0 }, lastIndex[2] = { 0 }, redraw = 1, scroll[2] = { 0 }, currentDrive[2];
	FILEINFO info;
	char currDirs[2][4][MAXPATH], workPath[2][MAXPATH];
	char filePath[2][MAXPATH];
	int maxDrives = DISK->GetNumDrives();
	char* curFN;
	int cs = 0;

	for (int i = 0; i < 4; i++)
	{
		sprintf(currDirs[0][i], "%c:\\", i + 'A');
		sprintf(currDirs[1][i], "%c:\\", i + 'A');
	}

	currentDrive[0] = path1[0] - 'A';
	strcpy_s(filePath[0], MAXPATH, currDirs[0][currentDrive[0]]);
	strcpy_s(workPath[0], MAXPATH, path1);
	Populate(workPath[0], 0, pattern);

	if (path2 != 0)
	{
		currentDrive[1] = path2[0] - 'A';
		strcpy_s(filePath[1], MAXPATH, currDirs[1][currentDrive[1]]);
		strcpy_s(workPath[1], MAXPATH, path2);
		Populate(workPath[1], 1, pattern);
	}

	MISC->SetTextMode(SMODE_BOLD | SMODE_240);
	//TEXT->SetTextColor(0, 7);
	//TEXT->ClearScreen();

	for(;;)
	{
		intoff();
		if (redraw)
		{
			if (redraw == 1 || (redraw == 2 && cs == 0))
			{
				DrawPanel(0, 1, WIDTH + 1, FILESSHOWN + 2, CLR_PANEL);
				TEXT->SetTextColor(SplitColor(CLR_PANELSEL));
				TEXT->SetCursorPosition((WIDTH / 2) - (strlen(workPath[0]) / 2) - 1, 1);
				TEXT->Write(" %s ", workPath[0]);

				char label[12] = { 0 };
				unsigned long id = 0;
				DrawPanel(0, FILESSHOWN + 2, WIDTH + 1, 4, CLR_PANEL);
				TEXTMAP[(FILESSHOWN + 2) * 80] = 0x8F00 | CLR_PANEL; //|-
				TEXTMAP[(FILESSHOWN + 2) * 80 + WIDTH] = 0x8A00 | CLR_PANEL; //-|
				TEXT->SetTextColor(SplitColor(CLR_PANEL));
				TEXT->SetCursorPosition(2, FILESSHOWN + 3);
				DISK->GetLabel(workPath[0][0], label, &id);
				TEXT->Write("Label: %04X-%04X, %s", id >> 16, id & 0xFFFF, label[0] ? label : "no name");
				id = DISK->GetFree(workPath[0][0]);
				TEXT->SetCursorPosition(2, FILESSHOWN + 4);
				TEXT->Write("Space: ");
				PrintComma(id);
				TEXT->Write(" bytes free");
			}

			if (path2 != 0 && (redraw == 1 || (redraw == 2 && cs == 1)))
			{
				DrawPanel(WIDTH + 1, 1, WIDTH + 1, FILESSHOWN + 2, CLR_PANEL);
				TEXT->SetTextColor(SplitColor(CLR_PANELSEL));
				TEXT->SetCursorPosition((WIDTH / 2) - (strlen(workPath[1]) / 2) - 1 + WIDTH + 1, 1);
				TEXT->Write(" %s ", workPath[1]);

				char label[12] = { 0 };
				unsigned long id = 0;
				DrawPanel(WIDTH + 1, FILESSHOWN + 2, WIDTH + 1, 4, CLR_PANEL);
				TEXTMAP[(FILESSHOWN + 2) * 80 + WIDTH + 1] = 0x8F00 | CLR_PANEL; //|-
				TEXTMAP[(FILESSHOWN + 2) * 80 + WIDTH + 1 + WIDTH] = 0x8A00 | CLR_PANEL; //-|
				TEXT->SetTextColor(SplitColor(CLR_PANEL));
				TEXT->SetCursorPosition(2 + WIDTH + 1, FILESSHOWN + 3);
				DISK->GetLabel(workPath[1][0], label, &id);
				TEXT->Write("Label: %04X-%04X, %s", id >> 16, id & 0xFFFF, label[0] ? label : "no name");
				id = DISK->GetFree(workPath[1][0]);
				TEXT->SetCursorPosition(2 + WIDTH + 1, FILESSHOWN + 4);
				TEXT->Write("Space: ");
				PrintComma(id);
				TEXT->Write(" bytes free");
			}

			for (int s = 0; s < 2; s++)
			{
				if (redraw == 2 && cs != s)
					continue;
				int o = (s == 0 ? 0 : WIDTH + 1);
				if (s == 1 && path2 == 0)
					break;
				curFN = &filenames[s][scroll[s] * 16];
				for (int i = 0; i < fileCt[s] && i < FILESSHOWN; i++)
				{
					TEXT->SetCursorPosition(1 + o, i + 2);
					TEXT->SetTextColor(SplitColor(CLR_PANELITEM));
					if (cs == s && index[s] == i + scroll[s])
						TEXT->SetTextColor(SplitColor(CLR_PANELSEL));
					printf("%-12s ", curFN);
					if (curFN[0] == '.' && curFN[1] == '.')
					{
						printf("         <UP>            ");
						curFN += 16;
						continue;
					}
					strcpy_s(filePath[s], MAXPATH, workPath[s]);
					if (filePath[s][strnlen_s(filePath[s], MAXPATH) - 1] != '\\') strkitten_s(filePath[s], MAXPATH, '\\');
					strcat_s(filePath[s], MAXPATH, curFN);
					DISK->FileStat(filePath[s], &info);
					if (info.fattrib & AM_DIRECTORY)
						printf("        <DIR> ");
					else
						printf("%13d ", info.fsize);
					int fdy = 1980 + (info.fdate >> 9);
					int fdm = (info.fdate >> 5) & 15;
					int fdd = info.fdate & 0x1F;
					printf(" %02d-%02d-%02d", fdy, fdm, fdd);
					curFN += 16;
				}
				TEXT->SetTextColor(0, 7);
				redraw = 0;
			}

			if (redraw < 2)
			{
				DrawKeys(keys);
				DrawMenu();
			}
		}
		else
		{
			int o = (cs == 0 ? 0 : WIDTH + 1);
			for (int i = 1; i < WIDTH; i++)
			{
				uint16_t* here = &TEXTMAP[o + ((2 + index[cs] - scroll[cs]) * 80)];
				here[i] &= ~0x00FF;
				here[i] |= CLR_PANELSEL;
				if (lastIndex[cs] != index[cs])
				{
					here = &TEXTMAP[o + ((2 + lastIndex[cs] - scroll[cs]) * 80)];
					here[i] &= ~0x00FF;
					here[i] |= CLR_PANELITEM;
				}
			}
			o = (cs == 1 ? 0 : WIDTH + 1);
			for (int i = 1; i < WIDTH; i++)
			{
				uint16_t* here = &TEXTMAP[o + ((2 + index[cs ^ 1] - scroll[cs ^ 1]) * 80)];
				here[i] &= ~0x00FF;
				here[i] |= CLR_PANELITEM;
			}
		}

//		for (int i = 0; i < 80; i++)
//			TEXTMAP[(29 * 80) + i] = 0x2007;
//		TEXT->SetCursorPosition(0, FILESSHOWN + 2);

		curFN = &filenames[cs][index[cs] * 16];
		//printf("%s>%s", workPath[cs], curFN);
		//printf("side %d, index %d, scroll %d, fileCt %d", cs, index[cs], scroll[cs], fileCt[cs]);
		vbl();

		while(1)
		{
			unsigned short key = REG_KEYIN;
			intoff();
			if ((key & 0xFF) > 0)
			{
				while(1) { if (REG_KEYIN == 0) break; }

				if (key == 0x0F && path2 != 0) //tab
				{
					cs ^= 1;
					//redraw = 1;
					break;
				}
				else if (key == 0xC8) //up
				{
					lastIndex[cs] = index[cs];
					if (index[cs] > 0)
					{
						index[cs]--;
						if (index[cs] < scroll[cs])
						{
							scroll[cs] -= FILESSHOWN;
							if (scroll[cs] < 0) scroll[cs] = 0;
							redraw = 1;
							break;
						}
					}
					else
					{
						index[cs] = fileCt[cs] - 1;
						if (fileCt[cs] > FILESSHOWN)
						{
							scroll[cs] = fileCt[cs] - FILESSHOWN;
							redraw = 1;
						}
						else
							scroll[cs] = 0;
						break;
					}
				}
				else if (key == 0xD0) //down
				{
					lastIndex[cs] = index[cs];
					if (index[cs] < fileCt[cs] - 1)
					{
						index[cs]++;
						if (index[cs] - scroll[cs] >= FILESSHOWN)
						{
							scroll[cs] += 1;
							if (scroll[cs] + FILESSHOWN > fileCt[cs]) scroll[cs] = fileCt[cs] - FILESSHOWN;
							redraw = 1;
							break;
						}
					}
					else
					{
						index[cs] = 0;
						scroll[cs] = 0;
						if (fileCt[cs] >= FILESSHOWN) redraw = 1;
						break;
					}
				}
				else if (key == 0xC9) //page up
				{
					index[cs] -= FILESSHOWN;
					scroll[cs] -= FILESSHOWN;
					if (index[cs] < 0) index[cs] = 0;
					if (scroll[cs] < 0) scroll[cs] = 0;
					redraw = 1;
					break;
				}
				else if (key == 0xD1) //page down
				{
					index[cs] += FILESSHOWN - 1;
					if (index[cs] >= fileCt[cs])
						index[cs] = fileCt[cs] - 1;
					if (index[cs] + scroll[cs] > FILESSHOWN)
					{
						scroll[cs] = index[cs];
						if (scroll[cs] + 1 >= fileCt[cs])
						{
							scroll[cs] -= FILESSHOWN - 1;
						}
					}
					redraw = 1;
					break;
				}
				else if (key == 0x1C) //enter
				{
					if (curFN[0] == '.' && curFN[1] == '.')
					{
						char *lastSlash = strrchr(workPath[cs], '\\');
						char justLeft[16] = { 0 };
						strcpy_s(justLeft, 16, lastSlash + 1);
						int32_t lsPos = lastSlash - workPath[cs];
						workPath[cs][lsPos] = 0;
						if (workPath[cs][0] == 0) strcpy_s(workPath[cs], MAXPATH, "\\");
						strcpy_s(currDirs[cs][currentDrive[cs]], MAXPATH, workPath[cs]);
						Populate(workPath[cs], cs, pattern);
						redraw = 1;
						index[cs] = 0;
						for (int r = 0; r < fileCt[cs]; r++)
						{
							if (!strcmp(&filenames[cs][r * 16], justLeft))
							{
								//DISK->FileStat(&filenames[cs][r * 16], &info);
								//if (info.fattrib & AM_DIRECTORY)
								{
									index[cs] = r;
									break;
								}
							}
						}
					}
					else
					{
						strcpy_s(filePath[cs], MAXPATH, workPath[cs]);
						if (filePath[cs][strnlen_s(filePath[cs], MAXPATH)-1] != '\\') strkitten_s(filePath[cs], MAXPATH, '\\');
						strcat_s(filePath[cs], MAXPATH, curFN);
						DISK->FileStat(filePath[cs], &info);
						if (info.fattrib & AM_DIRECTORY)
						{
							strcpy_s(workPath[cs], MAXPATH, filePath[cs]);
							strcpy_s(currDirs[cs][currentDrive[cs]], MAXPATH, workPath[cs]);
							Populate(workPath[cs], cs, pattern);
							redraw = 1;
							index[cs] = 0;
						}
						else
						{
							if (onSelect)
							{
								if (filenames[0] != 0)
								{
									free(filenames[0]);
									filenames[0] = 0;
								}
								if (filenames[1] != 0)
								{
									free(filenames[1]);
									filenames[1] = 0;
								}
								int32_t ret = onSelect(filePath[cs]);
								Populate(workPath[0], 0, pattern);
								if (path2 != 0)
									Populate(workPath[1], 1, pattern);
								if (ret == 1)
								{
									if (selection) strcpy_s(selection, MAXPATH, (const char*)filePath[cs]);
									return;
								}
								redraw = (ret == 2);
								if (redraw)
								{
									TEXT->SetTextColor(0, 7);
									TEXT->ClearScreen();
								}
							}
							else if (selection)
							{
								strcpy_s(selection, MAXPATH, (const char*)filePath[cs]);
								return;
							}
						}
					}
				}
				else if (key == 0x3B || key == 0x3C) //F1 or F2
				{
					int d = key - 0x3B;
					currentDrive[d] = SwitchDrive(d, currentDrive[d]);
					strcpy_s(filePath[d], MAXPATH, currDirs[d][currentDrive[d]]);
					strcpy_s(workPath[d], MAXPATH, filePath[d]);
					Populate(workPath[d], d, pattern);
					redraw = (cs == d) ? 2 : 1;
					index[d] = 0;
				}
				else if (key == 0x3D) //F3
					ShowError("Viewer implemented but not hooked up yet.");
				else if (key == 0x3E) //F4
					ShowError("Editor unlikely to be implemented.");
				else if (key == 0x3F) //F5
					ShowError("File copying not implemented yet.");
				else if (key == 0x40) //F6
					ShowError("Renaming and moving not implemented yet.");
				else if (key == 0x41) //F7
					ShowError("Directory creating not implemented yet.");
				else if (key == 0x42) //F8
					ShowError("File deleting not implemented yet.");
				else if (key == 0x43) //F9
					ShowError("File printing not implemented yet.");
				else if (key == 0x44) //F10
					OpenMenu();
//				else
//					printf("0x%X", key);
				break;
			}
		}
	}
}

int main(void)
{
	char path[MAXPATH];
	intoff();
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	REG_CARET = 0;
	while(1)
	{
		SelectFile("A:\\", "A:\\", "*.*", path, ShowFile);
	}
}
