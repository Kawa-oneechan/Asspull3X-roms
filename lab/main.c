#include "../ass.h"
#include "std.h"

IBios* interface;

void WaitForKey()
{
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
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

int32_t main1()
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);

	//GETCHAR
	while(1)
	{
		char c = getchar();
		printf("$%02X '%c'\n", c, c);
		if (c == '\e') break;
	}

	//GETS_S
	char input[64];
	putchar('>');
	gets_s(input, 64);
	printf("You said: \"%s\"\n", input);

	return 0;
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

	//printf("What's your name?\n");
	//gets(cwd, MAX_CWD);
	//printf("Hello %s.\n", cwd);
	//return 1;

	while(1)
	{
		DISK->GetCurrentDir(cwd, MAX_CWD);
		printf("%s>", (char*)&cwd);
		gets_s(input, MAX_INP);
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
	}
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

char ReadChar(FILE* file)
{
	char buffer[1];
	DISK->ReadFile(file, buffer, 1);
	return buffer[0];
}
short ReadShort(FILE* file)
{
	short buffer[1];
	DISK->ReadFile(file, buffer, 2);
	return buffer[0];
}
short ReadShortLE(FILE* file)
{
	unsigned char buffer[2];
	DISK->ReadFile(file, buffer, 2);
	return (buffer[1] << 8) | buffer[0];
}
int ReadInt(FILE* file)
{
	int buffer[1];
	DISK->ReadFile(file, buffer, 4);
	return buffer[0];
}
int ReadIntLE(FILE* file)
{
	unsigned char buffer[4];
	DISK->ReadFile(file, buffer, 4);
	return (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];
}
