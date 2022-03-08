#include "nav.h"

IBios* interface;

extern const uint8_t iconsTiles[];

#define MAXPATH 512
#define MAXFILES 512

#define WIDTH 39
#define HEIGHT 24
#define FILESSHOWN (HEIGHT-2)

void PrintComma(int32_t n)
{
    int32_t n2 = 0;
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
	const uint16_t abcd[] = { 30, 48, 46, 32 };
	static int numDrives = -1;
	static char driveTypes[4];

	if (numDrives == -1)
	{
		numDrives = 0;
		uint8_t* devices = (uint8_t*)0x02000000;
		for (char i = 0; i < 16; i++)
		{
			if (*(int16_t*)devices == 0x0144)
			{
				if (numDrives < 4)
					driveTypes[numDrives] = *(char*)&devices[5];
				numDrives++;
			}
			devices += 0x8000;
		}
	}

	tWindow* win = OpenWindow((WIDTH >> 1) - 2 + (WIDTH * which), 8, 9, numDrives + 2, CLR_DIALOG);
	for (int i = 0; i < numDrives; i++)
	{
		int16_t icon = ((0xB9 + (driveTypes[i] << 1)) << 8) | ((CLR_DIALOG >> 4) << 4);
		int16_t o = ((win->top + 1 + i) * 80) + win->left + 2;
		TEXTMAP[o++] = icon;
		TEXTMAP[o++] = icon + 0x100;
		o++;
		TEXTMAP[o++] = (('A' + i) << 8) | CLR_DIALOG;
		TEXTMAP[o++] = (':' << 8) | CLR_DIALOG;
	}
	int ret = now;
	Highlight(win->left + 1, win->top + 1 + ret, win->width - 4, 0x90);
	while (1)
	{
		uint16_t key = REG_KEYIN;
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
		uint16_t key = REG_KEYIN;
		intoff();
		if ((key & 0xFF) > 0)
		{
			while(1) { if (REG_KEYIN == 0) break; }
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

void InfoPanel(int panel, char* workPath, char* filename)
{
	char filePath[MAXPATH];
	FILEINFO info;
	int o = (panel == 0 ? 0 : WIDTH + 1);

	strcpy_s(filePath, MAXPATH, workPath);
	if (filePath[strnlen_s(filePath, MAXPATH) - 1] != '\\') strkitten_s(filePath, MAXPATH, '\\');
	strcat_s(filePath, MAXPATH, filename);
	DISK->FileStat(filePath, &info);

	char t[20][37] = { 0 };
	int i = 0;

	if (filename[0] == '.' && filename[1] == '.')
		sprintf(t[0], "Go up one directory");
	else
	{
		sprintf(t[i++], "@%s", filename);
		if (info.fattrib & AM_DIRECTORY)
			sprintf(t[i++], "Directory");
		else
			sprintf(t[i++], "File");
		if (info.fattrib & AM_READONLY) sprintf(t[i++], "Read-only");
		if (info.fattrib & AM_HIDDEN) sprintf(t[i++], "Hidden");
		if (info.fattrib & AM_SYSTEM) sprintf(t[i++], "System");
		if (info.fattrib & AM_ARCHIVE) sprintf(t[i++], "Archive");
		sprintf(t[i++], "0x%02X", info.fattrib);
	}

	for (i = 0; i < 20; i++)
	{
		TEXT->SetCursorPosition(o + 2, 3 + i);
		TEXT->SetTextColor(CLR_PANEL >> 4, t[i][0] == '@' ? 14 : (CLR_PANEL & 0x0F));
		printf("%-37s", t[i] + (t[i][0] == '@' ? 1 : 0));
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
	int views[2] = { 0, 0 };
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
		vbl();
		if (redraw)
		{
			if (redraw < 2)
			{
				DrawPanel(0, 1, WIDTH + 1, FILESSHOWN + 5, CLR_PANEL);
				DrawPanel(WIDTH + 1, 1, WIDTH + 1, FILESSHOWN + 5, CLR_PANEL);
			}

			for (int s = 0; s < 2; s++)
			{
				int o = (s == 0 ? 0 : WIDTH + 1);
				if (redraw == 2 && cs != s)
					continue;

				if (views[s] == 0)
				{
					TEXT->SetTextColor(SplitColor(CLR_PANEL));
					for (int i = 0; i < WIDTH - 1; i++)
						TEXTMAP[80 + o + i + 1] = 0x8300 | CLR_PANEL; //top edge
					TEXT->SetCursorPosition(o + (WIDTH / 2) - (strlen(workPath[s]) / 2), 1);
					TEXT->Write(" %s ", workPath[s]);

					char label[12] = { 0 };
					uint32_t id = 0;
					TEXTMAP[(FILESSHOWN + 2) * 80 + o] = 0xBD00 | CLR_PANEL; //|-
					for (int i = 0; i < WIDTH - 1; i++)
						TEXTMAP[(FILESSHOWN + 2) * 80 + o + i + 1] = 0x9000 | CLR_PANEL; //--
					TEXTMAP[(FILESSHOWN + 2) * 80 + o + WIDTH] = 0xBE00 | CLR_PANEL; //-|
					TEXT->SetTextColor(SplitColor(CLR_PANEL));
					TEXT->SetCursorPosition(2 + o + 1, FILESSHOWN + 3);
					DISK->GetLabel(workPath[s][0], label, &id);
					TEXT->Write("Label: %04X-%04X, %s", id >> 16, id & 0xFFFF, label[0] ? label : "no name");
					id = DISK->GetFree(workPath[s][0]);
					TEXT->SetCursorPosition(2 + o, FILESSHOWN + 4);
					TEXT->Write("Space: ");
					PrintComma(id);
					TEXT->Write(" bytes free");

					curFN = &filenames[s][scroll[s] * 16];
					for (int i = 0; i < FILESSHOWN; i++)
					{
						intoff();
						if (i >= fileCt[s])
						{
							TEXT->SetCursorPosition(1 + o, i + 2);
							TEXT->SetTextColor(SplitColor(CLR_PANELITEM));
							printf("%38c", ' ');
							continue;
						}

						strcpy_s(filePath[s], MAXPATH, workPath[s]);
						if (filePath[s][strnlen_s(filePath[s], MAXPATH) - 1] != '\\') strkitten_s(filePath[s], MAXPATH, '\\');
						strcat_s(filePath[s], MAXPATH, curFN);
						DISK->FileStat(filePath[s], &info);
						char icon[3] = "  ";
						if (curFN[0] == '.' && curFN[1] == '.')
						{
							//Leave it.
						}
						else if (info.fattrib & AM_DIRECTORY)
						{
							icon[0] = 0xB2;
							icon[1] = 0xB3;
						}
						else
						{
							icon[0] = 0xB4;
							icon[1] = 0xB5;
							char* ext = strrchr((const char*)filePath, '.') + 1;
							if (!strcmp(ext, "APP"))
							{
								icon[0] = 0xB6;
								icon[1] = 0xB7;
							}
						}
						TEXT->SetCursorPosition(1 + o, i + 2);
						TEXT->SetTextColor(SplitColor(CLR_PANELITEM));
						//if (cs == s && index[s] == i + scroll[s])
						//	TEXT->SetTextColor(SplitColor(CLR_PANELSEL));
						printf("%s %-12s ", icon, curFN);
						if (curFN[0] == '.' && curFN[1] == '.')
						{
							printf("      <UP>            ");
							curFN += 16;
							if (cs == s && index[s] == i + scroll[s])
								Highlight(o, i + 2, WIDTH + 1, CLR_PANELSEL);
							continue;
						}
						if (info.fattrib & AM_DIRECTORY)
							printf("     <DIR> ");
						else
							printf("%10d ", info.fsize);
						int fdy = 1980 + (info.fdate >> 9);
						int fdm = (info.fdate >> 5) & 15;
						int fdd = info.fdate & 0x1F;
						printf(" %02d-%02d-%02d", fdy, fdm, fdd);
						curFN += 16;
						if (cs == s && index[s] == i + scroll[s])
							Highlight(o, i + 2, WIDTH + 1, CLR_PANELSEL);
						if (cs == s && lastIndex[s] != index[s] && lastIndex[s] == i + scroll[s])
							Highlight(o, i + 2, WIDTH + 1, CLR_PANELITEM);
					}
				}
				else if (views[s] == 1)
				{
					TEXT->SetTextColor(SplitColor(CLR_PANEL));
					TEXT->SetCursorPosition(o + (WIDTH / 2) - 2 + 1, 1);
					TEXT->Write(" Info ");
					InfoPanel(s, workPath[s ^ 1], &filenames[s ^ 1][index[s ^ 1] * 16]);
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
			Highlight(o, 2 + lastIndex[cs] - scroll[cs], WIDTH + 1, CLR_PANELITEM);
			if (lastIndex[cs] != index[cs])
			{
				Highlight(o, 2 + index[cs] - scroll[cs], WIDTH + 1, CLR_PANELSEL);
				if (views[cs ^ 1] == 1)
					InfoPanel(cs ^ 1, workPath[cs], &filenames[cs][index[cs] * 16]);
			}
			o = (cs == 1 ? 0 : WIDTH + 1);
			Highlight(o, 2 + index[cs ^ 1] - scroll[cs ^ 1], WIDTH + 1, CLR_PANELITEM);
		}
		curFN = &filenames[cs][index[cs] * 16];

		//TODO: command prompt here?
		//TEXT->SetCursorPosition(0, 28);
		//printf("%s>%s", workPath[cs], "lol");

		while(1)
		{
			vbl();
			uint16_t key = REG_KEYIN;
			intoff();
			if ((key & 0xFF) > 0)
			{
				while(1) { if (REG_KEYIN == 0) break; }

				if (key & 0x200)
				{
					key &= 0xFF;
					for (int i = 0; i < NUMMENUS; i++)
					{
						if (key == menuBar[i].scan)
						{
							key = OpenMenu(i);
							goto HandleMenu;
							break;
						}
					}
				}
				else if (key == 0x0F && views[cs ^ 1] == 0) //tab
				{
					if (views[cs ^ 1] == 0)
						Highlight(cs ? WIDTH + 1 : 0, index[cs] + 2, WIDTH + 1, CLR_PANELITEM);
					cs ^= 1;
					redraw = 2;
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
							redraw = 2;
							break;
						}
					}
					else
					{
						index[cs] = fileCt[cs] - 1;
						if (fileCt[cs] > FILESSHOWN)
						{
							scroll[cs] = fileCt[cs] - FILESSHOWN;
							redraw = 2;
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
							redraw = 2;
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
					redraw = 2;
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
					redraw = 2;
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
						redraw = 2;
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
							redraw = 2;
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
					int oD = currentDrive[d];
					int nD = SwitchDrive(d, oD);
					if (oD != nD)
					{
						currentDrive[d] = nD;
						strcpy_s(filePath[d], MAXPATH, currDirs[d][currentDrive[d]]);
						strcpy_s(workPath[d], MAXPATH, filePath[d]);
						Populate(workPath[d], d, pattern);
						redraw = 2;
						DrawPanel(cs ? WIDTH + 1 : 0, 1, WIDTH + 1, FILESSHOWN + 5, CLR_PANEL);
						index[d] = 0;
					}
				}
				else if (key == 0x3D) //F3
				{ key = 11; goto HandleMenu; }
				else if (key == 0x3E) //F4
				{ key = 12; goto HandleMenu; }
				else if (key == 0x3F) //F5
				{ key = 13; goto HandleMenu; }
				else if (key == 0x40) //F6
				{ key = 14; goto HandleMenu; }
				else if (key == 0x41) //F7
				{ key = 15; goto HandleMenu; }
				else if (key == 0x42) //F8
				{ key = 16; goto HandleMenu; }
				else if (key == 0x43) //F9
				{ key = 17; goto HandleMenu; }
				else if (key == 0x44) //F10
				{
					key = OpenMenu(0);
HandleMenu:
					if (key > 100)
					{
						int side = 0;
						if (key > 110)
						{
							side = 1;
							key -= 10;
						}
						key -= 100;
						tMenuItem* m =  side ? rightMenu : leftMenu;
						tMenuItem* m2 = side ? leftMenu : rightMenu;
						switch (key)
						{
							case 1: //Files view
							case 2: //Info view
								views[side] = key - 1;
								m[0].state = 0;
								m[1].state = 0;
								m[2].state = 0;
								m[3].state = 0;
								m[key - 1].state = 2;
								if (views[side] != 0)
								{
									cs = side ^ 1;
									m2[1].state = 1;
									m2[2].state = 1;
									m2[3].state = 1;
								}
								else
								{
									m2[1].state = 0;
									m2[2].state = 0;
									m2[3].state = 0;
								}
								break;
						}
						redraw = 1;
					}
					else
					{
						switch (key)
						{
							case 11: //View
								ShowFile(curFN, false);
								redraw = 1;
								break;
							case 12: //Edit
								ShowError("Editor unlikely to be implemented.");
								break;
							case 13: //Copy
								ShowError("File copying not implemented yet.");
								break;
							case 14: //RenMov
								ShowError("Renaming and moving not implemented yet.");
								break;
							case 15: //Mkdir
								ShowError("Directory creating not implemented yet.");
								break;
							case 16: //Delete
								ShowError("File deleting not implemented yet.");
								break;
							case 17: //Print
								ShowError("File printing not implemented yet.");
								break;
							case 18: //Attrib
								ChangeAttributes(curFN);
								break;
						}
					}
				}
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
	MISC->DmaCopy(TEXTFONT + 0x2A00, (int8_t*)&iconsTiles, 512, DMA_BYTE);
	REG_CARET = 0;
	while(1)
	{
		SelectFile("A:\\", "A:\\", "*.*", path, ShowFile);
	}
}
