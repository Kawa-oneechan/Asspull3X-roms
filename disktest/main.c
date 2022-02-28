#include "../ass.h"
#include "../lab/std.h"
IBios* interface;

extern char *strrchr(const char *, int32_t);

#define MAXPATH 512
#define MAXFILES 512

#define WIDTH 39
#define HEIGHT 24

void WaitForKey()
{
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

typedef struct
{
	unsigned char left, top, width, height;
	unsigned short* bits;
} tWindow;

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

void DrawKeys(char** keys)
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

#define FILESSHOWN (HEIGHT-2)

void SelectFile(const char* path1, const char* path2, const char* pattern, char* selection, int32_t(*onSelect)(char*))
{
	static const char* keys[] = {
		"Help  ",
		"User  ",
		"View  ",
		"Edit  ",
		"Copy  ",
		"RenMov",
		"Mkdir ",
		"Delete",
		"Menu  ",
		"Quit  ",
	};

	int index[2] = { 0 }, lastIndex[2] = { 0 }, redraw = 1, scroll[2] = { 0 }, currentDrive[2];
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
			DrawPanel(0, 0, WIDTH + 1, FILESSHOWN + 2, 0x87);
			TEXT->SetTextColor(7, 8);
			TEXT->SetCursorPosition((WIDTH / 2) - (strlen(workPath[0]) / 2) - 1, 0);
			TEXT->Write(" %s ", workPath[0]);

			char label[12] = { 0 };
			unsigned long id = 0;
			DrawPanel(0, FILESSHOWN + 1, WIDTH + 1, 4, 0x87);
			TEXTMAP[(FILESSHOWN + 1) * 80] = 0x8F87; //|-
			TEXTMAP[(FILESSHOWN + 1) * 80 + WIDTH] = 0x8A87; //-|
			TEXT->SetTextColor(8, 7);
			TEXT->SetCursorPosition(2, FILESSHOWN + 2);
			DISK->GetLabel(workPath[0][0], label, &id);
			TEXT->Write("Label: %04X-%04X, %s", id >> 16, id & 0xFFFF, label[0] ? label : "no name");
			id = DISK->GetFree(workPath[0][0]);
			TEXT->SetCursorPosition(2, FILESSHOWN + 3);
			TEXT->Write("Space: ");
			PrintComma(id);
			TEXT->Write(" bytes free");

			if (path2 != 0)
			{
				DrawPanel(WIDTH + 1, 0, WIDTH + 1, FILESSHOWN + 2, 0x87);
				TEXT->SetTextColor(7, 8);
				TEXT->SetCursorPosition((WIDTH / 2) - (strlen(workPath[1]) / 2) - 1 + WIDTH + 1, 0);
				TEXT->Write(" %s ", workPath[1]);

				DrawPanel(WIDTH + 1, FILESSHOWN + 1, WIDTH + 1, 4, 0x87);
				TEXTMAP[(FILESSHOWN + 1) * 80 + WIDTH + 1] = 0x8F87; //|-
				TEXTMAP[(FILESSHOWN + 1) * 80 + WIDTH + 1 + WIDTH] = 0x8A87; //-|
				TEXT->SetTextColor(8, 7);
				TEXT->SetCursorPosition(2 + WIDTH + 1, FILESSHOWN + 2);
				DISK->GetLabel(workPath[1][0], label, &id);
				TEXT->Write("Label: %04X-%04X, %s", id >> 16, id & 0xFFFF, label[0] ? label : "no name");
				id = DISK->GetFree(workPath[1][0]);
				TEXT->SetCursorPosition(2 + WIDTH + 1, FILESSHOWN + 3);
				TEXT->Write("Space: ");
				PrintComma(id);
				TEXT->Write(" bytes free");
			}

			for (int s = 0; s < 2; s++)
			{
				int o = (s == 0 ? 0 : WIDTH + 1);
				if (s == 1 && path2 == 0)
					break;
				curFN = &filenames[s][scroll[s] * 16];
				for (int i = 0; i < fileCt[s] && i < FILESSHOWN; i++)
				{
					TEXT->SetCursorPosition(1 + o, i + 1);
					TEXT->SetTextColor(8, 15);
					if (cs == s && index[s] == i + scroll[s])
						TEXT->SetTextColor(9, 15);
					printf("%-12s ", curFN);
					if (curFN[0] == '.' && curFN[1] == '.')
					{
						printf("         <UP>            ");
						curFN += 16;
						continue;
					}
					else if (curFN[1] == ':')
					{
						printf("       <DISK>            ");
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

			DrawKeys(keys);
		}
		else
		{
			int o = (cs == 0 ? 0 : WIDTH + 1);
			for (int i = 1; i < WIDTH; i++)
			{
				uint16_t* here = &TEXTMAP[o + ((1 + index[cs] - scroll[cs]) * 80)];
				here[i] &= ~0x00FF;
				here[i] |= 0x009F;
				if (lastIndex[cs] != index[cs])
				{
					here = &TEXTMAP[o + ((1 + lastIndex[cs] - scroll[cs]) * 80)];
					here[i] &= ~0x00FF;
					here[i] |= 0x008F;
				}
			}
			o = (cs == 1 ? 0 : WIDTH + 1);
			for (int i = 1; i < WIDTH; i++)
			{
				uint16_t* here = &TEXTMAP[o + ((1 + index[cs ^ 1] - scroll[cs ^ 1]) * 80)];
				here[i] &= ~0x00FF;
				here[i] |= 0x008F;
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
		ShowError("Failed to malloc.");
		return 2;
	}
	FILE file;
	DISK->OpenFile(&file, filePath, FA_READ);
	DISK->ReadFile(&file, (void*)image, nfo.fsize);
	DISK->CloseFile(&file);
	if (image->BitDepth != 4 && image->BitDepth != 8)
	{
		ShowError("Weird bitdepth, not happening.");
		return 2;
	}
	DRAW->DisplayPicture(image);
	free(image);
	WaitForKey();
	return 2;
}

int32_t ShowText(char* filePath)
{
	int i, j, scroll = 0, lineCt = 0, redraw = 1;

	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	int32_t size = nfo.fsize;

	intoff();

	unsigned char* fileText = malloc(size);
	FILE file;
	DISK->OpenFile(&file, filePath, FA_READ);
	DISK->ReadFile(&file, (void*)fileText, nfo.fsize);
	DISK->CloseFile(&file);

	unsigned char* fullText = malloc(size + 1024);
	unsigned char *b = fileText;
	unsigned char *c = fullText;
	i = 0;
	while (*b != 0)
	{
		if (*b == '\r')
			b++;
		else if (*b == '\n')
		{
			*c++ = *b++;
			lineCt++;
			i = 0;
		}
		else
		{
			*c++ = *b++;
			i++;
			if (i == 80)
			{
				*c++ = '\n';
				lineCt++;
				i = 0;
			}
		}
	}

	free(fileText);

	b = fullText;
	TEXT->ClearScreen();

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
			printf(" %s \t%d/%d $%x, $%x", filePath, scroll, lineCt, b, b - fullText);
			intoff();
			c = b;
			int row = 1;
			int col = 0;
			while (row < 29 && *c != 0)
			{
				if (*c == '\n' && col < 80)
				{
					TEXTMAP[(row * 80) + col] = 0x0F04;
					row++;
					col = 0;
				}
				else if (col == 80)
				{
					row++;
					col = 0;
				}
				else
					TEXTMAP[(row * 80) + (col++)] = (*c << 8) | 0x07;
				c++;
			}
			redraw = 0;
		}

		unsigned short key = REG_KEYIN;
		//vbl();
		if ((key & 0xFF) > 0)
		{
			while(1) { if (REG_KEYIN == 0) break; }

			/*if (key == 0xCB) //left
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
			else*/ if (key == 0xC8) //up
			{
				if (scroll > 0)
				{
					b -= 2;
					while (b >= fullText && *b != '\n')
						b--;
					b++;
					scroll--;
					if (b < fullText)
					{
						b = fullText;
						scroll = 0;
					}
					redraw = 1;
				}
			}
			else if (key == 0xD0) //down
			{
				if (scroll < lineCt - 26)
				{
					while (b < fullText + size && *b != '\n')
						b++;
					b++;
					scroll++;
					redraw = 1;
				}
			}
			else if (key == 0x01) //esc
				break;
			else
				printf("%x", key);
		}
	}
	free(fullText);
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
		char msg[64];
		sprintf(msg, "Unknown file type \"%s\".", ext);
		ShowError(msg);
		return 3;
	}
	intoff();
	TEXT->SetTextColor(0, 7);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	return 2;
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
