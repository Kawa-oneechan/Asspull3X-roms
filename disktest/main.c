#include "../ass.h"
#include "../lab/std.h"
IBios* interface;

extern char *strrchr(const char *, int32_t);

#define MAXPATH 256
#define MAXFILES 512

void* LoadFile(const char* path, void* buffer, int32_t len)
{
	FILE file;
	FILEINFO nfo;
	int regs = REG_INTRMODE;
	REG_INTRMODE |= 0x80;
	int32_t ret = DISK->FileStat(path, &nfo);

	ret = DISK->OpenFile(&file, path, FA_READ);
	if (ret > 0)
	{
		REG_INTRMODE = regs;
		return (void*)ret;
	}

	void *target = buffer;
	for(;;)
	{
		ret = DISK->ReadFile(&file, target, 1024);
		if (ret < 0)
		{
			REG_INTRMODE = regs;
			return (void*)ret;
		}

		target += ret;
		if (ret < 1024)
			break;
	}

	DISK->CloseFile(&file);
	REG_INTRMODE = regs;
	return buffer;
}

void WaitForKey()
{
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

char filenames[MAXFILES][16] = {0};
int32_t fileCt = 0;

void Populate(const char* path, const char* pattern)
{
	int32_t ret;
	DIR dir;
	FILEINFO info;
	fileCt = 0;

	while (!HaveDisk())
	{
		TEXT->SetCursorPosition(0, 0);
		TEXT->SetTextColor(4, 15);
		printf("Please insert a disk.");
		WaitForKey();
	}

	ret = DISK->OpenDir(&dir, path);
	if (strnlen_s(path, MAXPATH) > 1)
	{
		strcpy_s(filenames[0], MAXPATH, "..");
		fileCt++;
	}
#ifdef ALLOW_DIRECTORIES
	for (;;)
	{
		ret = DISK->ReadDir(&dir, &info);
		if (ret != 0 || info.fname[0] == 0) break;
		if (info.fattrib & AM_DIRECTORY)
		{
			strncpy(filenames[fileCt], info.fname, 13);
			fileCt++;
		}
	}
#endif
	ret = DISK->FindFirst(&dir, &info, path, pattern);
	while(ret == 0 && info.fname[0])
	{
		if (info.fattrib & AM_HIDDEN) continue;
		strncpy(filenames[fileCt], info.fname, 13);
		fileCt++;
		ret = DISK->FindNext(&dir, &info);
	}
}

#define FILESSHOWN 20

void SelectFile(const char* path, const char* pattern, char* selection, int32_t(*onSelect)(char*), char* prompt)
{
	int32_t index = 0, lastIndex = 0, redraw = 1, scroll = 0;
	FILEINFO info;
	char filePath[MAXPATH], workPath[MAXPATH];

	strcpy_s(workPath, MAXPATH, path);
	Populate(workPath, pattern);

	TEXT->SetTextColor(0, 7);
	for(;;)
	{
		REG_INTRMODE |= 0x80;
		if (redraw)
		{
			TEXT->ClearScreen();
			TEXT->SetCursorPosition(0, 0);
			printf("%s\n", prompt ? prompt : "Select a file to open:");
			printf("> %s\n", workPath);

			for (int32_t i = 0; i < fileCt && i < FILESSHOWN; i++)
			{
				TEXT->SetTextColor(0, 7);
				if (index == i + scroll)
					TEXT->SetTextColor(1, 15);
				printf(" %-13s ", filenames[i + scroll]);
				TEXT->SetTextColor(0, 7);
				if (filenames[i + scroll][0] == '.' && filenames[i + scroll][1] == '.')
				{
					printf("    <UP>\n");
					continue;
				}
				strcpy_s(filePath, MAXPATH, workPath);
				if (filePath[strnlen_s(filePath, MAXPATH) - 1] != '/') strkitten_s(filePath, MAXPATH, '/');
				strcat_s(filePath, MAXPATH, filenames[i + scroll]);
				DISK->FileStat(filePath, &info);
				if (info.fattrib & AM_DIRECTORY)
					printf("   <DIR>\n");
				else
					printf("%8d\n", info.fsize);
			}
			TEXT->SetTextColor(0, 7);
			redraw = 0;
		}
		else
		{
			TEXT->SetCursorPosition(0, 2 + index - scroll);
			TEXT->SetTextColor(1, 15);
			printf(" %-13s \n", filenames[index]);
			TEXT->SetTextColor(0, 7);
			if (lastIndex != index)
			{
				TEXT->SetCursorPosition(0, 2 + lastIndex - scroll);
				printf(" %-13s \n", filenames[lastIndex]);
			}
		}
		TEXT->SetCursorPosition(0, FILESSHOWN + 4);
		printf("=>%s         ", filenames[index]);
		vbl();
		while(1)
		{
			unsigned short key = REG_KEYIN;
			REG_INTRMODE |= 0x80;
			if ((key & 0xFF) > 0)
			{
				while(1) { if (REG_KEYIN == 0) break; }

				if (key == 0xC8) //up
				{
					lastIndex = index;
					if (index > 0)
					{
						index--;
						if (index < scroll)
						{
							scroll -= FILESSHOWN;
							if (scroll < 0) scroll = 0;
							redraw = 1;
							break;
						}
					}
					else
					{
						index = fileCt - 1;
						if (fileCt > FILESSHOWN)
							scroll = fileCt - FILESSHOWN;
						else
							scroll = 0;
						redraw = 1;
						break;
					}
					//lastIndex = index;
					//if (index <= 0) index = fileCt;
					//index--;
				}
				else if (key == 0xD0) //down
				{
					lastIndex = index;
					if (index < fileCt - 1)
					{
						index++;
						if (index - scroll >= FILESSHOWN)
						{
							scroll += FILESSHOWN;
							if (scroll + FILESSHOWN > fileCt) scroll = fileCt - FILESSHOWN;
							redraw = 1;
							break;
						}
					}
					else
					{
						index = 0;
						scroll = 0;
						redraw = 1;
						break;
					}
					//lastIndex = index;
					//index++;
					//if (index >= fileCt) index = 0;
				}
				else if (key == 0xC9) //page up
				{
					index -= FILESSHOWN;
					scroll -= FILESSHOWN;
					if (index < 0) index = 0;
					if (scroll < 0) scroll = 0;
					redraw = 1;
					break;
				}
				else if (key == 0xD1) //page down
				{
					index += FILESSHOWN;
					scroll += FILESSHOWN;
					if (index + FILESSHOWN > fileCt) index = fileCt - FILESSHOWN;
					if (scroll + FILESSHOWN > fileCt) scroll = fileCt - FILESSHOWN;
					redraw = 1;
					break;
				}
				else if (key == 0x1C) //enter
				{
					if (filenames[index][0] == '.' && filenames[index][1] == '.')
					{
						char *lastSlash = strrchr(workPath, '/');
						int32_t lsPos = lastSlash - workPath;
						workPath[lsPos] = 0;
						Populate(workPath, pattern);
						redraw = 1;
						index = 0;
					}
					else
					{
						strcpy_s(filePath, MAXPATH, workPath);
						if (filePath[strnlen_s(filePath, MAXPATH)-1] != '/') strkitten_s(filePath, MAXPATH, '/');
						strcat_s(filePath, MAXPATH, filenames[index]);
						DISK->FileStat(filePath, &info);
						if (info.fattrib & AM_DIRECTORY)
						{
							strcpy_s(workPath, MAXPATH, filePath);
							Populate(workPath, pattern);
							redraw = 1;
							index = 0;
						}
						else
						{
							if (onSelect)
							{
								int32_t ret = onSelect(filePath);
								if (ret == 1)
								{
									if (selection) strcpy_s(selection, MAXPATH, (const char*)filePath);
									return;
								}
								redraw = (ret == 2);
							}
							else if (selection)
							{
								strcpy_s(selection, MAXPATH, (const char*)filePath);
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

int32_t startapp(char* filePath)
{
	void(*entry)(void) = (void*)0x01002020;
	//char* cartName = (char*)0x01002008;
	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	int32_t size = nfo.fsize;
	LoadFile((const char*)filePath, (void*)0x01002000, size);
	TEXT->ClearScreen();
	entry();
	return 2;
}

int32_t showpic(char* filePath)
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
	LoadFile((const char*)filePath, (void*)image, size);
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

int32_t showtext(char* filePath)
{
	int i, j, cur, scroll = 0, lineCt = 0, redraw = 1;
	char c;
	FILE* fd;
	char lines[MAXVIEWERLINES][MAXVIEWERLINELENGTH] = {0};
	REG_INTRMODE |= 0x80;
	fd = fopen(filePath, "r");
	TEXT->ClearScreen();
	printf("Loading %s...\n", filePath);
	i = 0;
	while (fgets(lines[i++], MAXVIEWERLINELENGTH, fd));
	lineCt = i;
	fclose(fd);
	while(1)
	{
		REG_INTRMODE |= 0x80;
		if (redraw)
		{
			TEXT->SetTextColor(0, 7);
			TEXT->ClearScreen();
			TEXT->SetTextColor(7, 0);
			for (j = 0; j < 80; j++)
				((int16_t*)MEM_VRAM)[j] = 0x70;
			printf(" %s \t%d/%d ", filePath, scroll, lineCt);
			REG_INTRMODE |= 0x80;
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
						((int16_t*)MEM_VRAM)[cur++] = (c << 8) | 0x07;
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

int32_t showfile(char* filePath)
{
	char* ext = strrchr(filePath, '.') + 1;
	//printf("showfile: \"%s\"\n", ext);
	if (!strcmp(ext, "TXT"))
		showtext(filePath);
	else if (!strcmp(ext, "API"))
		showpic(filePath);
	else if (!strcmp(ext, "APP"))
		startapp(filePath);
	else
	{
		TEXT->SetCursorPosition(0, 0);
		printf("Unknown file type \"%s\".         ", ext);
		WaitForKey();
	}
	REG_INTRMODE |= 0x80;
	TEXT->SetTextColor(0, 7);
	MISC->SetTextMode(SMODE_240);
	DRAW->ResetPalette();
	return 2;
}

int32_t main(void)
{
	interface = (IBios*)(0x01000000);
	char path[MAXPATH];
	REG_INTRMODE |= 0x80;
	MISC->SetTextMode(SMODE_240);
	DRAW->ResetPalette();
	while(1)
	{
		TEXT->ClearScreen();
		SelectFile("/", "*.*", path, showfile, NULL);
	}
}
