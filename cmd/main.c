#include "../ass.h"
#include "../lab/std.h"

#define MAX_CWD 256
#define MAXPATH 256
#define MAX_INP 256

IBios* interface;

extern char *strrchr(const char *, int32_t);
char cwd[MAX_CWD];

char *args[128];
int argc;

int HaveDisk(const char* path)
{
	DIR dir;
	int ret = DISK->OpenDir(&dir, path);
	if (ret == 3)
		return 0;
	return 1;
}

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
	while (REG_KEYIN != 0);
	while (REG_KEYIN == 0);
	while (REG_KEYIN != 0);
}

int32_t StartApp(char* filePath)
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

void UpperCase(char *from, char *to)
{
	while (*from)
	{
		printf("'%c'\n", *from);
		if (*from >= 'a' && *from <= 'z')
			*to = *from - 'a' + 'A';
		else
			*to = *from;
		to++;
		from++;
	}
}

int32_t ShowFile(int argc, char **args)
{
	if (argc == 0)
		return 0;
	char* filePath = args[0];
	char ext[4] = { 0 };
	UpperCase(strrchr(filePath, '.') + 1, ext);
	if (!strcmp(ext, "TXT"))
		; //ShowText(filePath);
	else if (!strcmp(ext, "API"))
	{
		ShowPic(filePath);
		MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
		TEXT->ClearScreen();
		TEXT->SetCursorPosition(0, 0);
		DRAW->ResetPalette();
	}
	else if (!strcmp(ext, "APP"))
		StartApp(filePath);
	else
	{
		//TEXT->SetCursorPosition(0, 0);
		printf("Unknown file type \"%s\".\n", ext);
		WaitForKey();
	}
	intoff();
	return 2;
}

int32_t ListFiles(int argc __attribute__((unused)), char **args __attribute__((unused)))
{
	int32_t ret;
	DIR dir;
	FILEINFO fno;
	uint32_t files = 0, dirs = 0, size = 0;
	char buff[MAXPATH];
	char *path = cwd;
	int mode = 1;

	if (!HaveDisk(path))
	{
		printf("No disk.\n\n");
		return 1;
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
	printf("\n\t\t%8i bytes free\n\n", 1474560 - size); //assuming a formatted 3�' HD diskette.
	return 2;
}

int32_t ChangeDirectory(int argc, char **args)
{
	if (argc == 0)
	{
		printf("%s\n", cwd);
		return 0;
	}
	//int ret = DISK->ChangeDir(path);
	int ret = DISK->ChangeDir(args[0]);
	if (ret != 0)
	{
		if (ret == 5) printf("Could not find the path.\n");
		else if (ret == 6) printf("The path name format is invalid.\n");
		else printf("Shit's fucked: %d\n", ret);
		return 1;
	}
	else
	{
		//printf("OK\n");
	}
	return 2;
}

int32_t ChangeDirUp(int argc __attribute__((unused)), char **args __attribute__((unused)))
{
	char* newArgs = "..";
	return ChangeDirectory(1, &newArgs);
}

typedef struct
{
	const char* keyword;
	int32_t(*handler)(int, char**);
} TCommand;

const TCommand commands[] =
{
	{ "cd", ChangeDirectory },
	{ "cd..", ChangeDirUp },
	{ "dir", ListFiles },
	{ "ls", ListFiles },
	{ "show", ShowFile },
	{ 0, 0 }
};

int32_t main()
{
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	char input[MAX_INP];
	const char del[] = " \t\n";
	char* trimmed = NULL;
	char* token, *ptr = NULL;

	while(1)
	{
		DISK->GetCurrentDir(cwd, MAX_CWD);
		if (cwd[0] == 0) sprintf(cwd, "No disk");
		printf("%s>", (char*)&cwd);
		gets_s(input, MAX_INP);
		trimmed = input;
		while (*trimmed == ' ')
			trimmed++;
		if (strlen(input) < 1)
			continue;
		token = strtok_r(trimmed, del, &ptr);

		argc = 0;
		while (1)
		{
			char *arg = strtok_r(NULL, del, &ptr);
			if (arg == 0)
				break;
			args[argc++] = arg;
		}
		for (int i = 0; commands[i].keyword; i++)
		{
			if (!strcmp(token, (char*)commands[i].keyword))
			{
				commands[i].handler(argc, args);
				break;
			}
		}

		/*
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
		*/
	}
}

