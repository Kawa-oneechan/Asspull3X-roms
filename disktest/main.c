#include "../ass.h"
IBios* interface;

#define STDIN  ((FILE*)-1)
#define STDOUT ((FILE*)-2)
#define EOF (-1)
#define MIN_GETLINE_CHUNK 64

static const char sctoasc[] = {
//Unshifted
//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	0,   0,   0,   0,   0,   0,   0,   0,   '\b','\t',0,   0,   0,   '\n',0,   0,   // 0x00
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   27,  0,   0,   0,   0,   // 0x10
	' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x20
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,   0,   0,   0,   0,   0,   // 0x30
	0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', // 0x40
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0,   0,   0,   0,   0,   // 0x50
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x80
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x90
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xa0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ';', '=', ',', '-', '.', '/', // 0xb0
	'`', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xc0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '[', '\\',']', '\'',0,   // 0xd0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xe0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xf0
//Shifted
//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	0,   0,   0,   0,   0,   0,   0,   0,   '\b','\t',0,   0,   0,   '\n',0,   0,   // 0x00
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   27,  0,   0,   0,   0,   // 0x10
	' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x20
	')', '!', '@', '#', '$', '%', '^', '^', '&', '*', '(', 0,   0,   0,   0,   0,   // 0x30
	0,   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', // 0X40
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0,   0,   0,   0,   0,   // 0X50
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x80
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x90
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xa0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ':', '+', '<', '_', '>', '?', // 0xb0
	'~', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xc0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '{', '|', '}', '\"',0,   // 0xd0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xe0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0    // 0xf0
};

//stdio
int fputc(int c, FILE* file)
{
	if (file == STDOUT)
	{
		TEXT->WriteChar((char)c);
		return c;
	}
	unsigned char _c = (unsigned char)c;
	DISK->WriteFile(file, &_c, 1);
	return c;
}

#define putc(c, file) fputc(c, file)
#define putchar(c) fputc(c, STDOUT)

int fputs(const char* s, FILE* file)
{
	if (file == STDOUT)
	{
		TEXT->Write(s);
		return 0;
	}
	while(*s) fputc(*s++, file);
	return 0;
}

#define puts(s) TEXT->Write(s); TEXT->WriteChar('\n');

int16_t fgetc(FILE* file)
{
	if (file != STDIN)
	{
		if (DISK->FileEnd(file)) return EOF;
		char c;
		DISK->ReadFile(file, &c, 1);
		return c;
		//char buffer[1];
		//DISK->ReadFile(stream, buffer, 1);
		//return buffer[0];
	}
	unsigned short key = 0;
	while (1)
	{
		key = REG_KEYIN;
		if ((key & 0xFF) > 0)
			break;
	}
	while (1)
	{
		if ((REG_KEYIN & 0xFF) == 0)
			break;
	}
	if (file == STDIN) TEXT->WriteChar(sctoasc[key]);
	return sctoasc[key];
}

#define getc(file) fgetc(file)
#define getchar() fgetc(STDIN)

int getdelim(char** linePtr, int* n, char delim, FILE* file)
{
	register int charsAvailable;
	char* readPos;
	if (*linePtr == NULL)
	{
		*n = MIN_GETLINE_CHUNK;
		*linePtr = malloc(*n);
	}
	charsAvailable = *n;
	readPos = *linePtr;
	while(1)
	{
		int c = getc(file);
		if (file == STDIN && c == '\b')
		{
			if (readPos > *linePtr) readPos--;
			continue;
		}
		if (c == '\r' && file != STDIN) continue;
		else if (c == -1) break; //End of file.
		if (charsAvailable < 2)
		{
			if (*n > MIN_GETLINE_CHUNK) *n *= 2;
			else *n += MIN_GETLINE_CHUNK;
			charsAvailable = *n + *linePtr - readPos;
			*linePtr = realloc(*linePtr, *n);
			readPos = *n - charsAvailable + *linePtr;
		}
		*readPos++ = c;
		charsAvailable--;
		if (c == delim)
		{
			if (file != STDIN) readPos--;
			break;
		}
	}
	*readPos = '\0';
	return (readPos - *linePtr);
}
#define getline(l,n,f) getdelim(l,n,'\n',f)

char* fgets(char* s, int n, FILE* file)
{
	char* _s = s;
	int c;
	c = fgetc(file);
	if (c == EOF)
		return 0;
	*_s++ = (char)c;
	--n;
	while(--n)
	{
		c = fgetc(file);
		if (c == '\n' || c == EOF)
			break;
		*_s++ = (char)c;
	}
	*_s = 0;
	return s;
}
#define gets(s,n) fgets(s,n,STDIN)

int fread(void* data, int size, int count, FILE* file)
{
	if (size == 0 || count == 0) return 0;
	int ret = 0;
	for (; ret < count; ret++)
	{
		if (DISK->ReadFile(file, data, size) < 0) break;
		data += size;
	}
	return ret;
}

int fwrite(void* data, int size, int count, FILE* file)
{
	if (size == 0 || count == 0) return 0;
	int ret = 0;
	for (; ret < count; ret++)
	{
		if (DISK->WriteFile(file, data, size) < 0) break;
		data += size;
	}
	return ret;
}

#define sprintf(b,f,rest...) TEXT->Format(b,f, ## rest)

FILE* fopen(const char* filename, const char* opentype)
{
	char mode = 0;
	switch (opentype[0])
	{
		case 'r': mode = FA_READ; break;
		case 'w': mode = FA_WRITE | FA_CREATE_NEW; break;
		case 'a': mode = FA_WRITE | FA_CREATE_NEW | FA_OPEN_APPEND; break;
	}
	if (opentype[1] == '+') return NULL; //NOPE :D
	FILE* file = malloc(sizeof(FILE));
	int ret = DISK->OpenFile(file, filename, mode);
	if (ret == 0) return file;
	free(file);
	return (FILE*)ret;
}

int fclose(FILE* file)
{
	if (file->flag & FA_WRITE) DISK->FlushFile(file);
	if (DISK->CloseFile(file)) return EOF;
	free(file);
	return 0;
}

#define feof(f) DISK->FileEnd(f)
#define ftell(f) DISK->FilePosition(f)
#define fseek(f,o,w) DISK->SeekFile(f,o,w)
#define rewind(f) DISK->SeekFile(f, 0, SEEK_SET)
//stdio

extern char * strtok_r (char *newstring, const char *delimiters, char **save_ptr);
extern int strlen (char *s);
extern int strcmp (char *a, char *b);
extern char *strrchr(const char *, int32_t);

#define MAXPATH 256
#define MAXFILES 512

/*void WriteFileTime(FILINFO fno)
{
	short date = fno.fdate;
	short time = fno.ftime;
	printf("%04d\\-%02d%\\-%02d %02d:%02d:%02d",
		((date >>  9) & 0x7F) + 1980,
		 (date >>  5) & 0x0F,
		 (date >>  0) & 0x1F,
		 (time >> 11) & 0x1F,
		 (time >>  5) & 0x3F,
		 (time >>  0) & 0x0F
	);
}*/

void ListFiles(const char* path, int32_t mode)
{
	int32_t ret;
	DIR dir;
	FILEINFO fno;
	uint32_t files = 0, dirs = 0, size = 0;
	char buff[MAXPATH];
	if (!HaveDisk())
	{
		printf("No disk.");
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
/*
void PrintTextFile(const char* path)
{
	FILE file;
	char line[256];
	int32_t ret = DISK->OpenFile(&file, path, FA_READ);
	if (ret > 0)
	{
		printf("Error %i opening \"%s\": %s\n\n", ret, path, DISK->FileErrStr(ret));
		return;
	}
	while (!DISK->FileEnd(&file))
	{
		DISK->FileReadLine(&file, line, sizeof line);
		ws(line);
	}
	DISK->CloseFile(&file);
	ws("\n\n");
}*/

/*
int32_t LoadFile(const char* path, void* target)
{
	FIL file;
	int32_t ret = f_open(&file, path, FA_READ);
	uint32_t bytesRead, totalRead = 0;
	if (ret > 0) return ret;
	for(;;)
	{
		ret = f_read(&file, target, 1024, &bytesRead);
		if (ret > 0) return ret;
		target += bytesRead;
		totalRead += bytesRead;
		if (bytesRead < 1024)
			break;
	}
	//printf("%d", totalRead);
	f_close(&file);
	return 0;
}*/

void* LoadFile(const char* path, void* buffer, int32_t len)
{
	FILE file;
	FILEINFO nfo;
	int32_t ret = DISK->FileStat(path, &nfo);
	//printf("%i, %i -> %i\n", ret, nfo.fsize, len);
	//void* mem = malloc(nfo.fsize);
	//printf("%i\n", mem);
	//return 0;
	//if (mem == 0) return 0;
	ret = DISK->OpenFile(&file, path, FA_READ);
	//void* target = mem;
	if (ret > 0) return (void*)ret;
	/*for(;;)
	{
		ret = f_read(&file, target, 1024, &bytesRead);
		if (ret > 0) return (void*)ret;
		target += bytesRead;
		if (bytesRead < 1024)
			break;
	}*/
	if (nfo.fsize < len) len = nfo.fsize;
	//printf("LoadFile: gonna try reading %d bytes to %x.\n", len, buffer);
	ret = DISK->ReadFile(&file, buffer, len);
	if (ret < 0) return (void*)ret;
	DISK->CloseFile(&file);
	return buffer;
}

//TImageFile img;

/*void EnumerateFiles(const char* path, const char* pattern, int32_t(*enumerator)(TFileInfo*))
{
	int32_t ret;
	DIR dir;
	FILEINFO info;
	ret = DISK->FindFirst(&dir, &info, "", "*.api");
	while (ret == 0 && info.fname[0])
	{
		if (enumerator(&info))
			break;
		ret = DISK->FindNext(&dir, &info);
	}
}

int32_t apiEnumerator(TFileInfo* info)
{
	printf("%s\n", info->fname);
	if (info->fname[0] == 'P')
	{
		ws("(Hold up, that's our cue to stop enumerating! Returning a 1...)\n");
		return 1;
	}
	return 0;
}*/

void WaitForKey()
{
	while (REG_KEYIN == 0);
	vbl();
	while (REG_KEYIN != 0);
}

/*typedef struct TSelectFileParms
{
	char* path;
	char* pattern;
	int32_t(*onSelect)(char*);
	void(*onRedraw)(void*, char*, int32_t);
	void(*onRefresh)(void*, char*, int32_t, int32_t);
	char* extra1;
	char* extra2;
	char* extra3;
} TSelectFileParms;*/

char filenames[MAXFILES][16] = {0};
int32_t fileCt = 0;


void Populate(const char* path, const char* pattern)
{
	int32_t ret;
	DIR dir;
	FILEINFO info;
	fileCt = 0;
	//printf("(Populating %s)\n", path); WaitForKey();
	ret = DISK->OpenDir(&dir, path);
	//printf("(Populate: OpenDir(\"%s\") %d -- %s)\n", path, ret, DISK->FileErrStr(ret));
	if (strnlen_s(path, MAXPATH) > 1)
	{
		strcpy_s(filenames[0], MAXPATH, "..");
		fileCt++;
	}
#ifdef ALLOW_DIRECTORIES
	for (;;)
	{
		ret = DISK->ReadDir(&dir, &info);
		//printf("(Populate: ReadDir %d -- %s)\n", ret, DISK->FileErrStr(ret));
		if (ret != 0 || info.fname[0] == 0) break;
		if (info.fattrib & AM_DIRECTORY)
		{
			strncpy(filenames[fileCt], info.fname, 13);
			//printf("(Populate: #%d = %s (dir))\n", fileCt, filenames[fileCt]);
			fileCt++;
		}
	}
#endif
	ret = DISK->FindFirst(&dir, &info, path, pattern);
	//printf("(Populate: FindFirst %d -- %s)\n", ret, DISK->FileErrStr(ret));
	while(ret == 0 && info.fname[0])
	{
		if (info.fattrib & AM_HIDDEN) continue;
		strncpy(filenames[fileCt], info.fname, 13);
		//printf("(Populate: #%d = %s)\n", fileCt, filenames[fileCt]);
		fileCt++;
		ret = DISK->FindNext(&dir, &info);
		//printf("(Populate: FindNext %d -- %s)\n", ret, DISK->FileErrStr(ret));
	}
	//WaitForKey();
}

void SelectFile(const char* path, const char* pattern, char* selection, int32_t(*onSelect)(char*), char* prompt)
{
	int32_t index = 0, lastIndex = 0, redraw = 1;
	FILEINFO info;
	char filePath[MAXPATH], workPath[MAXPATH];

	strcpy_s(workPath, MAXPATH, path);
	Populate(workPath, pattern);

	TEXT->SetTextColor(0, 7);
	for(;;)
	{
		if (redraw)
		{
			TEXT->ClearScreen();
			TEXT->SetCursorPosition(0, 0);
			printf("%s\n", prompt ? prompt : "Select a file to open:");
			printf("> %s\n", workPath);

			for (int32_t i = 0; i < fileCt; i++)
			{
				TEXT->SetTextColor(0, 7);
				if (index == i)
					TEXT->SetTextColor(1, 15);
				printf(" %-13s ", filenames[i]);
				TEXT->SetTextColor(0, 7);
				if (filenames[i][0] == '.' && filenames[i][1] == '.')
				{
					printf("    <UP>\n");
					continue;
				}
				strcpy_s(filePath, MAXPATH, workPath);
				if (filePath[strnlen_s(filePath, MAXPATH)-1] != '/') strkitten_s(filePath, MAXPATH, '/');
				strcat_s(filePath, MAXPATH, filenames[i]);
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
			TEXT->SetCursorPosition(0, 2+index);
			TEXT->SetTextColor(1, 15);
			printf(" %-13s \n", filenames[index]);
			TEXT->SetCursorPosition(0, 2+lastIndex);
			TEXT->SetTextColor(0, 7);
			printf(" %-13s \n", filenames[lastIndex]);
		}
		while(1)
		{
			unsigned short key = REG_KEYIN;
			if ((key & 0xFF) > 0)
			{
				while(1) { if (REG_KEYIN == 0) break; }

				if (key == 0x28) //up
				{
					lastIndex = index;
					index++;
					if (index == fileCt) index = 0;
				}
				else if (key == 0x26) //down
				{
					lastIndex = index;
					if (index == 0) index = fileCt;
					index--;
				}
				else if (key == 0x0D) //enter
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
				//25-28 -> left up right down
				break;
			}
		}
	}
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
	//WaitForKey();
	//TEXT->ClearScreen();
	//DRAW->ResetPalette();
	//MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	//REG_SCREENFADE = 0;
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
	/*
	FILE file;
	char kilo[1024] = {0};
	int32_t ret = DISK->OpenFile(&file, filePath, FA_READ);
	if (ret > 0)
	{
		TEXT->SetCursorPosition(0, 0);
		printf("Error %i opening \"%s\": %s\n\n", ret, filePath, DISK->FileErrStr(ret));
		return 0;
	}
	TEXT->ClearScreen();
	while (!DISK->FileEnd(&file))
	{
		DISK->ReadFile(&file, kilo, sizeof kilo);
		printf(kilo);
	}
	DISK->CloseFile(&file);
	*/
	WaitForKey();
	return 2;
}

int32_t showfile(char* filePath)
{
	char* ext = strrchr(filePath, '.') + 1;
	//printf("showfile: \"%s\"\n", ext);
	if (!strcmp(ext, "TXT"))
		return showtext(filePath);
	else if (!strcmp(ext, "API"))
		return showpic(filePath);
	else
	{
		TEXT->SetCursorPosition(0, 0);
		printf("Unknown file type \"%s\".         ", ext);
		WaitForKey();
	}
	return 2;
}



void ScanFiles(char* path)
{
	int res;
	DIR dir;
	unsigned int i;
	static FILEINFO fno;
	res = DISK->OpenDir(&dir, path);
	if (res == 0)
	{
		for (;;)
		{
			res = DISK->ReadDir(&dir, &fno);
			if (res != 0 || fno.fname[0] == 0)
				break;
			if (fno.fattrib & AM_DIRECTORY)
			{
				continue;
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);
				ScanFiles(path);
				path[i] = 0;
			}
			else
			{
				printf("%s/%s\n", path, fno.fname);
			}
		}
		DISK->CloseDir(&dir);
	}
}

void slideshow()
{
	int ret;
	DIR dir;
	FILEINFO info;
	TImageFile* image = NULL;
	ret = DISK->FindFirst(&dir, &info, "0:", "*.api");
	while(ret == 0 && info.fname[0])
	{
		//dpf("%s\n", info.fname);
		image = malloc(info.fsize);
		if (image == NULL)
		{
			continue;
		}
		LoadFile((const char*)info.fname, (void*)image, info.fsize);
		if (image->BitDepth != 4 && image->BitDepth != 8)
		{
			free(image);
			continue;
		}
		//MISC->DmaClear((void*)0x0E000000, 0, 640 * 480, DMA_SHORT);
		DRAW->DisplayPicture(image);
		//PALETTE[15] = 0x7FFF;
		//DRAW->DrawString(info.fname, 9, 9, 0);
		//DRAW->DrawString(info.fname, 8, 8, 15);
		DRAW->FadeFromBlack();
		free(image);

		//MISC->WaitForVBlanks(60);
		WaitForKey();
		DRAW->FadeToBlack();

		ret = DISK->FindNext(&dir, &info);
	}
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	TEXT->ClearScreen();
	printf("The end!\n");
}

int32_t main(void)
{
	interface = (IBios*)(0x01000000);
	//MISC->SetTextMode(SMODE_240);
	//TEXT->ClearScreen();
	//REG_VBLANKMODE = 0;

	slideshow(); return 0;

	//char buff[MAXPATH];
	//strcpy(buff, "0:");
	//ScanFiles(buff); WaitForKey();

	//ListFiles("/", 1); WaitForKey();

	/*printf("malloc test...");
	void* test = malloc(16);
	printf(" done: %x\n", test);
	printf("free test...");
	free(test);
	printf(" done: %x\n", test);
	WaitForKey();

	ListFiles("/"); WaitForKey();

	char path[MAXPATH];
	SelectFile("/", "*.*", path, NULL, NULL);
	printf("\n\n(You chose \"%s\".)", path); WaitForKey();
	//Menu(path, "*.api");*/
	//SelectFile("", "*.api", NULL, showpic, "Select an image to view:");
	//SelectFile("", "*.*", NULL, showfile, "Select a file to view:");
	/*TSelectFileParms myParms = {
		"/", "*.api", showpic, SelectFileEx_Redraw, SelectFileEx_Refresh,
		"Select an image to view:", NULL, NULL
	};
	SelectFileEx(NULL, &myParms);*/

	/*PrintTextFile("bullshit.txt");

	int32_t ret;
	DIR dir;
	FILEINFO info;
	ws("Now listing all images on the disk, in system order, with FindFirst/Next:\n");
	ret = DISK->FindFirst(&dir, &info, "", "*.api");
	while (ret == 0 && info.fname[0])
	{
		printf("%s\n", info.fname);
		ret = DISK->FindNext(&dir, &info);
	}
	ws("And now listing them again with a Win32-style enumeration callback:\n");
	EnumerateFiles("", "*.api", apiEnumerator);
	ws("...because why not, right?\n");*/

	//REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	/*TEXT->SetCursorPosition(0, 0);
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();*/

	//ws("zeroes are good.\n");
	//int32_t ret;

	/* ret = *///f_mount(&FatFs, "", 1);
	//ws("f_mount: "); wi(ret); wc('\n');

	/*TImageFile *img = LoadFile("shiny.api");
	printf("BitDepth: %d\nColorCount: %d\nWidth: %d\nHeight: %d\nStride: %d\nByteSize: %d\n", img->BitDepth, img->ColorCount + 1, img->Width, img->Height, img->Stride, img->ByteSize);
	printf("Expect color palette at %08x, indicated at %08x.\n", 0x18, img->ColorOffset);
	printf("Expect pixel data at %08x, indicated at %08x.\n", img->ColorOffset + ((img->ColorCount + 1) * 2), img->ImageOffset);
	printf("Expect byte size %d, indicated as %d.\n", img->Width * img->Height, img->ByteSize);
	REG_SCREENMODE = SMODE_BMP2 | SMODE_320 | SMODE_240;
	DRAW->DisplayPicture(img);
	//interface->miscLibrary->DmaCopy((void*)0x0E000000, ((char*)img + img->ImageOffset), img->ByteSize, DMA_INT);
	//interface->miscLibrary->DmaCopy((void*)0x0E100000, ((char*)img + img->ColorOffset), (img->ColorCount + 1) * 2, DMA_SHORT);
	free(img);*/

	/*ListFiles(0);
	ListFiles("/jack");
	PrintTextFile("bullshit.txt");
	PrintTextFile("bullshi.txt");
	ListFiles(0);
	ws("kek\n");*/

	/*char label[24];
	ret = f_getlabel("", label, 0);
	ws("f_getlabel: "); wi(ret); ws(": \""); ws(label); ws("\"\n");

	ret = f_setlabel("FUCKER");
	ws("f_setlabel: "); wi(ret); wc('\n');

	ret = f_getlabel("", label, 0);
	ws("f_getlabel: "); wi(ret); ws(": \""); ws(label); ws	("\"\n");*/

	/*FIL file;
	char line[256];
	ret = f_open(&file, "bullshit.txt", FA_READ);
	ws("f_open: "); wi(ret); wc('\n');
	while (f_gets(line, sizeof line, &file))
		ws(line);
	ret = f_close(&file);
	ws("f_close: "); wi(ret); wc('\n');*/

	/*FIL file;
	ret = f_open(&file, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS);
	ws("f_open: "); wi(ret); wc('\n');

	uint32_t bytes;
	ret = f_write(&file, "It works!\r\n", 11, &bytes);
	ws("f_write: "); wi(ret); wc('\n');

	ret = f_close(&file);
	ws("f_close: "); wi(ret); wc('\n');*/


	//if (f_open(&file, "newfile.txt", FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
	//	f_write(&file, "It works!\r\n", 11, &bytes);
	//	f_close(&file);
	//}

	while(1);
}
