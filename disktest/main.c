#include "../ass.h"
#include "../lab/std.h"
IBios* interface;

extern char *strrchr(const char *, int32_t);

#define MAXPATH 512
#define MAXFILES 512

#define WIDTH 39
#define HEIGHT 21

void WaitForKey()
{
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

//char filenames[MAXFILES][16] = {0};
char* filenames[2] = { 0 };
int32_t fileCt[2] = { 0 };

void Populate(const char* path, int side, const char* pattern)
{
	int32_t ret;
	DIR dir;
	FILEINFO info;
	fileCt[side] = 0;
	char* curFN = 0;

	if (filenames[side] == 0)
	{
		filenames[side] = (char*)malloc(MAXFILES * 16);
	}
	curFN = filenames[side];

	//Always add other drives
	int numDrives = DISK->GetNumDrives();
	for (int i = 0; i < numDrives; i++)
	{
		sprintf(curFN, "%c:", 'A' + i);
		curFN += 16;
		fileCt[side]++;
	}

tryOpenDir:
	ret = DISK->OpenDir(&dir, path);
	if (ret)
	{
		TEXT->SetCursorPosition(0, 0);
		TEXT->SetTextColor(4, 15);
		printf("Disk error reading %s: ", path);
		if (ret == 3)
			printf("No disk inserted?");
		else
			printf(DISK->FileErrStr(ret));
		TEXT->SetTextColor(0, 7);
		printf("\nAbort or Retry? >");
		while (1)
		{
			char k = getchar();
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

	for (;;)
	{
		ret = DISK->ReadDir(&dir, &info);
		if (ret != 0 || info.fname[0] == 0) break;
		if (info.fattrib & AM_DIRECTORY)
		{
			strncpy(curFN, info.fname, 13);
			curFN += 16;
			fileCt[side]++;
		}
	}

	ret = DISK->FindFirst(&dir, &info, path, pattern);
	while(ret == 0 && info.fname[0])
	{
		if (info.fattrib & AM_HIDDEN) continue;
		strncpy(curFN, info.fname, 13);
		curFN += 16;
		fileCt[side]++;
		ret = DISK->FindNext(&dir, &info);
	}
}

#define FILESSHOWN (HEIGHT-2)
#define TEXTMAP ((int16_t*)MEM_VRAM)

void SelectFile(const char* path1, const char* path2, const char* pattern, char* selection, int32_t(*onSelect)(char*))
{
	int32_t index[2] = { 0 }, lastIndex[2] = { 0 }, redraw = 1, scroll[2] = { 0 }, currentDrive[2];
	FILEINFO info;
	char currDirs[2][4][MAXPATH], workPath[2][MAXPATH];
	char filePath[2][MAXPATH];
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
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();

	for(;;)
	{
		intoff();
		if (redraw)
		{
			TEXTMAP[0] = 0x9387;
			TEXTMAP[WIDTH] = 0x8B87;
			TEXTMAP[(FILESSHOWN + 1) * 80] = 0x8C87;
			TEXTMAP[((FILESSHOWN + 1) * 80) + WIDTH] = 0x9287;
			for (int i = 1; i < WIDTH; i++)
			{
				TEXTMAP[i] = 0x9087;
				TEXTMAP[((FILESSHOWN + 1) * 80) + i] = 0x9087;
			}
			for (int i = 1; i <= FILESSHOWN; i++)
			{
				TEXTMAP[(i * 80)] = 0x8987;
				TEXTMAP[(i * 80)+WIDTH] = 0x8987;
			}
			if (fileCt[0] < FILESSHOWN)
			{
				for (int i = fileCt[0]; i <= FILESSHOWN; i++)
					for (int j = 1; j < WIDTH; j++)
						TEXTMAP[(i * 80) + j] = 0x2087;
			}
			TEXT->SetTextColor(7, 8);
			TEXT->SetCursorPosition((WIDTH / 2) - (strlen(workPath[0]) / 2) - 1, 0);
			TEXT->Write(" %s ", workPath[0]);

			if (path2 != 0)
			{
				TEXTMAP[WIDTH+1] = 0x9387;
				TEXTMAP[79] = 0x8B87;
				TEXTMAP[((FILESSHOWN + 1) * 80) + WIDTH + 1] = 0x8C87;
				TEXTMAP[((FILESSHOWN + 1) * 80) + 79] = 0x9287;
				for (int i = 1; i < WIDTH; i++)
				{
					TEXTMAP[i + WIDTH + 1] = 0x9087;
					TEXTMAP[((FILESSHOWN + 1) * 80) + i + WIDTH + 1] = 0x9087;
				}
				for (int i = 1; i <= FILESSHOWN; i++)
				{
					TEXTMAP[(i * 80) + WIDTH + 1] = 0x8987;
					TEXTMAP[(i * 80) + 79] = 0x8987;
				}
				if (fileCt[1] < FILESSHOWN)
				{
					for (int i = fileCt[1]; i <= FILESSHOWN; i++)
						for (int j = 1; j < WIDTH; j++)
							TEXTMAP[(i * 80) + WIDTH + 1 + j] = 0x2087;
				}
				TEXT->SetTextColor(7, 8);
				TEXT->SetCursorPosition((WIDTH / 2) - (strlen(workPath[1]) / 2) - 1 + WIDTH + 1, 0);
				TEXT->Write(" %s ", workPath[1]);
			}

			for (int s = 0; s < 2; s++)
			{
				int o = (s == 0 ? 0 : WIDTH + 1);
				if (s == 1 && path2 == 0)
					break;
				curFN = &filenames[s][scroll[s] * 16];
				for (int32_t i = 0; i < fileCt[s] && i < FILESSHOWN; i++)
				{
					TEXT->SetCursorPosition(1 + o, i + 1);
					TEXT->SetTextColor(8, cs == s ? 15 : 7);
					if (index[s] == i + scroll[s])
						TEXT->SetTextColor(cs == s ? 9 : 1, 15);
					printf("%-15s ", curFN);
					if (curFN[0] == '.' && curFN[1] == '.')
					{
						printf("     <UP>             ");
						curFN += 16;
						continue;
					}
					else if (curFN[1] == ':')
					{
						printf("   <DISK>             ");
						curFN += 16;
						continue;
					}
					strcpy_s(filePath[s], MAXPATH, workPath[s]);
					if (filePath[s][strnlen_s(filePath[s], MAXPATH) - 1] != '\\') strkitten_s(filePath[s], MAXPATH, '\\');
					strcat_s(filePath[s], MAXPATH, curFN);
					DISK->FileStat(filePath[s], &info);
					if (info.fattrib & AM_DIRECTORY)
						printf("    <DIR> ");
					else
						printf("%9d ", info.fsize);
					int fdy = 1980 + (info.fdate >> 9);
					int fdm = (info.fdate >> 5) & 15;
					int fdd = info.fdate & 0x1F;
					printf("  %02d-%02d-%02d", fdy, fdm, fdd);
					curFN += 16;
				}
				TEXT->SetTextColor(0, 7);
				redraw = 0;
			}
		}
		else
		{
			int o = (cs == 0 ? 0 : WIDTH + 1);
			for (int i = 1; i < WIDTH; i++)
			{
				int16_t* here = &TEXTMAP[o + ((1 + index[cs] - scroll[cs]) * 80)];
				here[i] &= ~0x00FF;
				here[i] |= 0x009F;
				if (lastIndex[cs] != index[cs])
				{
					here = &TEXTMAP[o + ((1 + lastIndex[cs] - scroll[cs]) * 80)];
					here[i] &= ~0x00FF;
					here[i] |= 0x008F;
				}
			}
		}

		for (int i = 0; i < 80; i++)
			TEXTMAP[(28 * 80) + i] = 0x2007;
		TEXT->SetCursorPosition(0, FILESSHOWN + 2);

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
					redraw = 1;
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
						int32_t lsPos = lastSlash - workPath[cs];
						workPath[cs][lsPos] = 0;
						if (workPath[cs][0] == 0) strcpy_s(workPath[cs], MAXPATH, "\\");
						strcpy_s(currDirs[cs][currentDrive[cs]], MAXPATH, workPath[cs]);
						Populate(workPath[cs], cs, pattern);
						redraw = 1;
						index[cs] = 0;
					}
					else if (curFN[1] == ':')
					{
						currentDrive[cs] = curFN[0] - 'A';
						strcpy_s(filePath[cs], MAXPATH, currDirs[cs][currentDrive[cs]]);
						strcpy_s(workPath[cs], MAXPATH, filePath[cs]);
						Populate(workPath[cs], cs, pattern);
						redraw = 1;
						index[cs] = 0;
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
				break;
			}
		}
	}
}

int32_t StartApp(char* filePath)
{
	void(*entry)(void) = (void*)0x01002020;
	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	FILE file;
	DISK->OpenFile(&file, filePath, FA_READ);
	DISK->ReadFile(&file, (void*)0x01002000, nfo.fsize);
	TEXT->ClearScreen();
	entry();
	WaitForKey();
	return 2;
}

int32_t ShowPic(char* filePath)
{
	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	int32_t size = nfo.fsize;

	TEXT->SetCursorPosition(0, 12);

	TImageFile* image = malloc(size);
	if (image == NULL)
	{
		printf("Failed to malloc.\n");
		WaitForKey();
		return 2;
	}
	FILE file;
	DISK->OpenFile(&file, filePath, FA_READ);
	DISK->ReadFile(&file, (void*)image, nfo.fsize);
	DISK->CloseFile(&file);
	if (image->BitDepth != 4 && image->BitDepth != 8)
	{
		printf("...yeah no.");
		WaitForKey();
		return 2;
	}
	DRAW->DisplayPicture(image);
	free(image);
	WaitForKey();
	return 2;
}

#define MAXVIEWERLINES 1024
#define MAXVIEWERLINELENGTH 81
#define MAXLINESSHOWN 29
#define SCROLLBY 1

int32_t ShowText(char* filePath)
{
	int i, j, cur, scroll = 0, lineCt = 0, redraw = 1;
	char c;
	FILE* fd;
	char lines[MAXVIEWERLINES][MAXVIEWERLINELENGTH] = {0};
	intoff();
	fd = fopen(filePath, "r");
	TEXT->ClearScreen();
	printf("Loading %s...\n", filePath);
	i = 0;
	while (fgets(lines[i++], MAXVIEWERLINELENGTH, fd));
	lineCt = i;
	fclose(fd);
	while(1)
	{
		intoff();
		if (redraw)
		{
			TEXT->SetTextColor(0, 7);
			TEXT->ClearScreen();
			TEXT->SetTextColor(1, 11);
			for (j = 0; j < 80; j++)
				TEXTMAP[j] = 0x1B;
			printf(" %s \t%d/%d ", filePath, scroll, lineCt);
			intoff();
			for (i = 0; i < MAXLINESSHOWN; i++)
			{
				if (i + scroll < lineCt)
				{
					cur = 80 * (i + 1);
					for (j = 0; j < 80; j++)
					{
						c = lines[i+scroll][j];
						if (c == 0) break;
						if (c == '\n') break;
						if (c == '\r') continue;
						if (c == '\t') c = ' ';
						TEXTMAP[cur++] = (c << 8) | 0x07;
					}
				}
			}
			redraw = 0;
		}

		unsigned short key = REG_KEYIN;
		//vbl();
		if ((key & 0xFF) > 0)
		{
			while(1) { if (REG_KEYIN == 0) break; }

			if (key == 0xCB) //left
			{
				if (scroll > 0)
				{
					scroll -= 10;
					if (scroll < 0)
						scroll = 0;
					redraw = 1;
				}
			}
			else if (key == 0xCD) //right
			{
				if (scroll + MAXLINESSHOWN < lineCt)
				{
					scroll += 10;
					redraw = 1;
				}
			}
			else if (key == 0xD0) //up
			{
				if (scroll + MAXLINESSHOWN < lineCt)
				{
					scroll += SCROLLBY;
					redraw = 1;
				}
			}
			else if (key == 0xC8) //down
			{
				if (scroll > 0)
				{
					scroll -= SCROLLBY;
					if (scroll < 0)
						scroll = 0;
					redraw = 1;
				}
			}
			else if (key == 0x01) //esc
				return 2;
			else
				printf("%x", key);
		}
	}
	WaitForKey();
	return 2;
}

int32_t ShowFile(char* filePath)
{
	char* ext = strrchr(filePath, '.') + 1;
	if (!strcmp(ext, "TXT"))
		ShowText(filePath);
	else if (!strcmp(ext, "API"))
		ShowPic(filePath);
	else if (!strcmp(ext, "APP"))
		StartApp(filePath);
	else
	{
		TEXT->SetCursorPosition(0, 0);
		printf("Unknown file type \"%s\".         ", ext);
		WaitForKey();
	}
	intoff();
	TEXT->SetTextColor(0, 7);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	return 2;
}

int32_t main(void)
{
	interface = (IBios*)(0x01000000);
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
