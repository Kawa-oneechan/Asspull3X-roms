#include "../ass.h"
#include "../ass-std.h"
IBios* interface;

int LoadFile(const char* path, char** buffer, int32_t len)
{
	FILE file;
	FILEINFO nfo;
	int32_t ret = DISK->FileStat(path, &nfo);
	ret = DISK->OpenFile(&file, path, FA_READ);
	if (ret > 0) return -ret;
	if (*buffer == NULL)
	{
		*buffer = malloc(nfo.fsize);
		len = nfo.fsize;
	}
	if (nfo.fsize < len) len = nfo.fsize;
	ret = DISK->ReadFile(&file, buffer, len);
	if (ret < 0) return ret;
	DISK->CloseFile(&file);
	return len;
}

void WaitForKey()
{
	while (REG_KEYIN == 0);
	while (REG_KEYIN != 0);
}


int32_t ShowPic(char* filePath)
{
	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	int32_t size = nfo.fsize;

	TImageFile* image = malloc(size);
	if (image == NULL)
	{
		printf("Failed to malloc.\n");
		WaitForKey();
		return 1;
	}
	//printf("Image malloc @ %p\n", image);

	LoadFile((const char*)filePath, (void*)image, size);

	/*printf("BitDepth: %d\nFlags: %d\nWidth: %d\nHeight: %d\nStride: %d\nByteSize: %d\n", image->BitDepth, image->Flags, image->Width, image->Height, image->Stride, image->ByteSize);
	unsigned char* debug = (unsigned char*)image;
	for (int32_t sixteens = 0; sixteens < 4; sixteens++)
	{
		printf("\n%08X:", debug);
		for (int32_t i = 0; i < 16; i++)
			printf(" %02X", *debug++);
	}*/

	if (image->BitDepth != 4 && image->BitDepth != 8)
	{
		printf("...yeah no.");
		WaitForKey();
		return 2;
	}
	//WaitForKey();
	//DRAW->FadeToWhite();
	DRAW->DisplayPicture(image);
	//DRAW->FadeFromWhite();
	free(image);
	WaitForKey();
	TEXT->ClearScreen();
	DRAW->ResetPalette();
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	REG_SCREENFADE = 0;
	return 0;
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
	char* lines[1024] = {0};
	fd = fopen(filePath, "r");
	TEXT->ClearScreen();
	//printf("Loading %s...\n", filePath);
	//printf("Loading lines...\n");
	i = 0;
	lines[i] = (char*)malloc(MAXVIEWERLINELENGTH);
	while (fgets(lines[i], MAXVIEWERLINELENGTH, fd))
	{
		//printf("%i. %s\n", i, lines[i]);
		i++;
		lines[i] = (char*)malloc(MAXVIEWERLINELENGTH);
	}
	lineCt = i;
	fclose(fd);
	//printf("Done.");
	//WaitForKey();
	while(1)
	{
		if (redraw)
		{
			TEXT->SetTextColor(0, 7);
			TEXT->ClearScreen();
			TEXT->SetTextColor(7, 0);
			for (j = 0; j < 80; j++)
				((int16_t*)MEM_VRAM)[j] = 0x70;
			printf(" %s \t%d/%d ", filePath, scroll, lineCt);
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
					//printf("%s\n", lines[i + scroll]);
				}
			}
			redraw = 0;
		}

		unsigned short key = REG_KEYIN;
		if ((key & 0xFF) > 0)
		{
			while(1) { if (REG_KEYIN == 0) break; }

			if (key == 0x25) //left
			{
				if (scroll > 0)
				{
					scroll -= 10;
					if (scroll < 0)
						scroll = 0;
					redraw = 1;
				}
			}
			else if (key == 0x27) //right
			{
				if (scroll + MAXLINESSHOWN < lineCt)
				{
					scroll += 10;
					redraw = 1;
				}
			}
			else if (key == 0x28) //up
			{
				if (scroll + MAXLINESSHOWN < lineCt)
				{
					scroll += SCROLLBY;
					redraw = 1;
				}
			}
			else if (key == 0x26) //down
			{
				if (scroll > 0)
				{
					scroll -= SCROLLBY;
					if (scroll < 0)
						scroll = 0;
					redraw = 1;
				}
			}
			else if (key == 0x1B) //esc
				return 2;
			else
				printf("%x", key);
		}
	}
	WaitForKey();
	return 0;
}

int32_t ShowFile(char* filePath)
{
	char* ext = strrchr(filePath, '.') + 1;
	//printf("showfile: \"%s\"\n", ext);
	if (!strcmp(ext, "TXT"))
		return ShowText(filePath);
	else if (!strcmp(ext, "API"))
		return ShowPic(filePath);
	else
	{
		TEXT->SetCursorPosition(0, 0);
		printf("Unknown file type \"%s\".         ", ext);
		WaitForKey();
	}
	return 2;
}

#define MAX_CWD 256
#define MAXPATH 256
#define MAX_INP 256

void ListFiles(const char* path, int32_t mode)
{
	int32_t ret;
	DIR dir;
	FILEINFO fno;
	uint32_t files = 0, dirs = 0, size = 0;
	char buff[MAXPATH];
	if (!HaveDisk())
	{
		printf("No disk.\n\n");
		return;
	}
	ret = DISK->GetLabel(buff);
	printf(" Volume name: %s\n", (buff[0] ? buff : "none"));
	if (path == 0)
	{
		path = buff;
		strcpy_s(buff, MAXPATH, "/");
	}
	printf(" Directory of %s:\n\n", path);
	ret = DISK->OpenDir(&dir, path);
	if (ret == 0)
	{
		for (;;)
		{
			ret = DISK->ReadDir(&dir, &fno);
			if (ret != 0 || fno.fname[0] == 0)
				break;

			if (mode == 0)
			{
				printf("  %04d\\-%02d%\\-%02d %02d:%02d:%02d",
					((fno.fdate >>  9) & 0x7F) + 1980,
					 (fno.fdate >>  5) & 0x0F,
					 (fno.fdate >>  0) & 0x1F,
					 (fno.ftime >> 11) & 0x1F,
					 (fno.ftime >>  5) & 0x3F,
					 (fno.ftime >>  0) & 0x0F
				);

				if (fno.fattrib & AM_DIRECTORY)
				{
					dirs++;
					printf("  %-14s (dir)\n", fno.fname);
				}
				else
				{
					size += fno.fsize;
					if (fno.fattrib & AM_HIDDEN)
						continue;
					files++;
					printf("  %-14s %i\n", fno.fname, fno.fsize);
				}
			}
			else if (mode == 1)
			{
				if (fno.fattrib & AM_DIRECTORY)
				{
					dirs++;
					TEXT->Format(buff, "[%s]", fno.fname);
					printf("%-14s\t", buff);
				}
				else
				{
					size += fno.fsize;
					if (fno.fattrib & AM_HIDDEN)
						continue;
					files++;
					printf("%-14s\t", fno.fname);
				}
			}
		}
	}
	ret = DISK->CloseDir(&dir);
	printf("\n\t\t%8i bytes used", size);
	printf("\n\t\t%8i bytes free\n\n", 1474560 - size); //assuming a formatted 3½' HD diskette.
}

int32_t main()
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	char cwd[MAX_CWD];
	char input[MAX_INP];
	const char del[] = " \t\n";
	char* trimmed = NULL;
	char* token, *ptr = NULL;
	char path[MAXPATH];
	REG_INTRMODE = 0;

	while(1)
	{
		DISK->GetCurrentDir(cwd, MAX_CWD);
		printf("%s>", (char*)&cwd);
		gets(input, MAX_INP);
		//printf("<len:%d>", strlen(raw));
		trimmed = input;
		while (*trimmed == ' ')
			trimmed++;
		//printf("<len:%d>", strlen(trimmed));
		if (strlen(input) < 1)
			continue;
		//printf("<%s>\n", line);
		token = strtok_r(trimmed, del, &ptr);
		//printf("[%s]\n", token);
		if (!strcmp(token, "cd"))
		{
			token = strtok_r(NULL, del, &ptr);
			/*if (!strcmp(token, ".."))
			{
				strcpy_s(path, MAXPATH, cwd);
				char *lastSlash = strrchr(path, '/');
				int32_t lsPos = lastSlash - path + 1;
				path[lsPos] = 0;
			}
			else
			*/{
				/*printf("(CD: current path is '%s', cwd is '%s')", path, cwd);
				strcpy_s(path, MAXPATH, cwd);
				printf("(CD: strcpy(cwd) -> '%s')", path);
				strcat_s(path, MAXPATH, token);
				printf("(CD: strcat(token) -> '%s')", path);
				strcat_s(path, MAXPATH, "/");
//				printf("(CD: strcat(\"/\") -> '%s')", path);*/

			}
			//printf("(CD: %s)", path);
			printf("(CD: %s)", token);
			//int ret = DISK->ChangeDir(path);
			int ret = DISK->ChangeDir(token);
			if (ret != 0)
			{
				if (ret == 5) printf("Could not find the path.\n");
				else if (ret == 6) printf("The path name format is invalid.\n");
				else printf("Shit's fucked: %d\n", ret);
			} else
			{
				printf("OK\n");
				//strcpy_s(cwd, MAXPATH, path);
				//DISK->GetCurrentDir(cwd, MAX_CWD);
			}
		}
		else if(!strcmp(token, "dir"))
		{
			ListFiles(cwd, 1);
		}
		else if(!strcmp(token, "ren"))
		{
			char* renFrom;
			char* renTo;
			token = strtok_r(NULL, del, &ptr);
			renFrom = token;
			token = strtok_r(NULL, del, &ptr);
			renTo = token;
			DISK->RenameFile(renFrom, renTo);
		}
		else if(!strcmp(token, "del"))
		{
			token = strtok_r(NULL, del, &ptr);
			DISK->UnlinkFile(token);
		}
		else if(!strcmp(token, "show"))
		{
			token = strtok_r(NULL, del, &ptr);
			ShowFile(token);
		}
	}
}

