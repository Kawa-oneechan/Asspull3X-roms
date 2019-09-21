#include "../ass.h"
IBios* interface;

/*

#define REG_BLITFUNC		*(volatile int32_t*)(MEM_IO+0x0100)
#define REG_BLITADDRA		*(volatile int32_t*)(MEM_IO+0x0104)
#define REG_BLITADDRB		*(volatile int32_t*)(MEM_IO+0x0108)
#define REG_BLITLENGTH		*(volatile int32_t*)(MEM_IO+0x010C)
#define REG_BLITKEY			*(volatile int8_t*)(MEM_IO+0x0110)

int32_t main()
{
	interface = (IBios*)(0x01000000);
	MISC->SetBitmapMode256(SMODE_320 | SMODE_240);
	TEXT->ClearScreen();

	REG_BLITADDRA = 0x0E;
	REG_BLITADDRB = 0x0E000000 + ((4 * 320) + 4);
	REG_BLITLENGTH = 256;
	REG_BLITFUNC =	(320 << 20) | //Target stride
					(20  <<  8) | //Source stride
					//(0   <<  7) | //Target 4-bits
					//(0   <<  6) | //Source 4-bits
					//(0   <<  5) | //Colorkey
					(0   <<  5) | //Width
					(1   <<  4) | //Strideskip
					0x2; //Clear

	REG_BLITADDRA = 0x06;
	REG_BLITADDRB = 0x0E000000 + ((5 * 320) + 5);
	REG_BLITLENGTH = 190;
	REG_BLITFUNC =	(320 << 20) | //Target stride
					(18  <<  8) | //Source stride
					//(0   <<  7) | //Target 4-bits
					//(0   <<  6) | //Source 4-bits
					//(0   <<  5) | //Colorkey
					(0   <<  5) | //Width
					(1   <<  4) | //Strideskip
					0x2; //Clear

	REG_BLITADDRA = 0x0D;
	REG_BLITADDRB = 0x0E000000 + ((5 * 320) + 34);
	REG_BLITLENGTH = 64;
	REG_BLITFUNC =	(320 << 20) | //Target stride
					(64  <<  8) | //Source stride
					//(0   <<  7) | //Target 4-bits
					//(0   <<  6) | //Source 4-bits
					//(0   <<  5) | //Colorkey
					(0   <<  5) | //Width
					(1   <<  4) | //Strideskip
					0x2; //Clear

	REG_BLITADDRA = 0x0C;
	REG_BLITADDRB = 0x0E000000 + ((7 * 320) + 32);
	REG_BLITLENGTH = 64;
	REG_BLITFUNC =	(320 << 20) | //Target stride
					(1   <<  8) | //Source stride
					//(0   <<  7) | //Target 4-bits
					//(0   <<  6) | //Source 4-bits
					//(0   <<  5) | //Colorkey
					(0   <<  5) | //Width
					(1   <<  4) | //Strideskip
					0x2; //Clear

	REG_BLITADDRA = 0x0B;
	REG_BLITADDRB = 0x0E000000 + ((6 * 320) + 33);
	REG_BLITLENGTH = 48;
	REG_BLITFUNC =	(321 << 20) | //Target stride
					(1   <<  8) | //Source stride
					//(0   <<  7) | //Target 4-bits
					//(0   <<  6) | //Source 4-bits
					//(0   <<  5) | //Colorkey
					(0   <<  5) | //Width
					(1   <<  4) | //Strideskip
					0x2; //Clear

	REG_BLITADDRA = 0x0A;
	REG_BLITADDRB = 0x0E000000 + ((7 * 320) + 28);
	REG_BLITLENGTH = 128;
	REG_BLITFUNC =	(320 << 20) | //Target stride
					(2   <<  8) | //Source stride
					//(0   <<  7) | //Target 4-bits
					//(0   <<  6) | //Source 4-bits
					//(0   <<  5) | //Colorkey
					(0   <<  5) | //Width
					(1   <<  4) | //Strideskip
					0x2; //Clear

}
*/

void WaitForKey()
{
	while (REG_KEYIN == 0);
	while (REG_KEYIN != 0);
}

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
int fgetc(FILE* file)
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
/*
#ifdef printf
#undef printf
#endif
__attribute((format (printf, 1, 2)))
int printf(const char* format, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	TEXT->Format(buffer, format, args);
	char *b = buffer;
	while (*b) TEXT->WriteChar(*b++);
	va_end(args);
	return buffer - b;
}
__attribute((format (printf, 2, 3)))
int fprintf(FILE* file, const char* format, ...)
{
	char buffer[1024];
	va_list args;
	va_start(args, format);
	TEXT->Format(buffer, format, args);
	char *b = buffer;
	while (*b) fputc(*b++, file);
	va_end(args);
	return b - buffer;
}
*/
#define sprintf(b,f,res...) TEXT->Format(b,f, ## rest)
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

	//printf("What's your name?\n");
	//gets(cwd, MAX_CWD);
	//printf("Hello %s.\n", cwd);
	//return 1;

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

#define likely(x)	__builtin_expect((x),1)
#define unlikely(x)	__builtin_expect((x),0)

void UnRLE(char* rle, char* pix, int size)
{
	int len = 0;
	char c = 0;
	while (size)
	{
		c = *rle++;
		if (likely(c & 0xC0))
		{
			len = (c & 0x3F);
			c = *rle++;
			while(len--)
			{
				*pix++ = c;
				size--;
			}
		}
		else
		{
			*pix++ = c;
			size--;
		}
	}
}

//#define WITH_BLITTERCHIP
TImageFile* LoadPCX(const char* path)
{
	FILE pcx;
	DISK->OpenFile(&pcx, path, FA_READ);
	unsigned int c = ReadChar(&pcx);
	if (c != 10) return (TImageFile*)-1; //Not a PCX file?
	c = ReadChar(&pcx);
	if (c != 5) return (TImageFile*)-2; //Wrong version
	c = ReadChar(&pcx);
	if (c != 1) return (TImageFile*)-3; //Wrong encoding
	c = ReadChar(&pcx);
	if (c != 8) return (TImageFile*)-4; //Not 256 colors
	/*int windowLeft = */ReadShortLE(&pcx);
	/*int windowTop = */ReadShortLE(&pcx);
	int width = ReadShortLE(&pcx) + 1;
	int height = ReadShortLE(&pcx) + 1;
	//printf("window: %d %d?\n", windowRight, windowBottom);
	int size = width * height;
	if (width % 8 != 0) return (TImageFile*)-10; //Not divisible
	/*int hRes = */ReadShortLE(&pcx);
	/*int vRes = */ReadShortLE(&pcx);
	DISK->SeekFile(&pcx, 48, SEEK_CUR); //ColorMap
	ReadChar(&pcx); //reserved
	c = ReadChar(&pcx);
	if (c != 1) return (TImageFile*)-11; //Not single-plane
	int stride = ReadShortLE(&pcx);
	//printf("stride: %d\n", stride);
	c = ReadChar(&pcx);
	if (c != 1) return (TImageFile*)-20; //Unexpected palette interpretation
	DISK->SeekFile(&pcx, -769, SEEK_END);
	c = ReadChar(&pcx);
	//printf("%#x, %#x\n", pcx.fptr, c);
	if (c != 12) return (TImageFile*)-21; //Unexpected palette marker byte
	TImageFile* img = malloc(sizeof(TImageFile) + (256 * 2) + size);
	img->Identifier = 0x41494D47;
	img->BitDepth = 8;
	img->Flags = 0;
	img->Width = width;
	img->Height = height;
	img->Stride = stride;
	img->ByteSize = size;
	img->ColorOffset = 24;
	img->ImageOffset = 536;
	unsigned char rgb[3];
	short* colors = (short*)(img + 24);
	char* pixels = (char*)(img + 536);
	for (int i = 0; i < 256; i++)
	{
		DISK->ReadFile(&pcx, rgb, 3);
		colors[i] = ((rgb[2] >> 3) << 10) | ((rgb[1] >> 3) << 5) | (rgb[0] >> 3);
		//printf("color %d: %#02x%02x%02x -> %#4x\n", i, rgb[0], rgb[1], rgb[2], colors[i]);
		//if (i % 8 == 0) WaitForKey();
	}
	DISK->SeekFile(&pcx, 128, SEEK_SET);
	//printf("pixels: %d of them (%dx%d)\n", size, width, height);
	char* pix = pixels;
#ifdef WITH_READCHAR
	unsigned char len;
	int i = size;
	while (i)
	{
		c = ReadChar(&pcx);
		//printf("%x ", c);
		if ((c & 0xC0) == 0xC0)
		{
			len = (unsigned char)(c & 0x3F);
			c = ReadChar(&pcx);
			//printf("[run %#x for %d]", c, len);
			for (int j = 0; j < len; j++)
			{
				*pix++ = c;
				i--;
			}
		}
		else
		{
			*pix++ = c;
			i--;
		}
	}
	DISK->CloseFile(&pcx);
#else
	int rleSize = pcx.obj.objsize - pcx.fptr;
	char* rleStream = malloc(rleSize);
	char* rle = rleStream;
	DISK->ReadFile(&pcx, rleStream, rleSize);
	DISK->CloseFile(&pcx);
#ifdef WITH_BLITTERCHIP
	REG_BLITADDRA = (int32_t)rle;
	REG_BLITADDRB = (int32_t)pix;
	REG_BLITLENGTH = size;
	REG_BLITFUNC = 0x0003FC04;
#else
	UnRLE(rle, pix, size);
#endif
	free(rleStream);
#endif
	//while(1);
	return img;
}

/*
int32_t main()
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240);
	DRAW->ResetPalette();
	TImageFile* pcx = LoadPCX("gnmin7.pcx");
	//printf("%p\n", pcx);
	//printf("BitDepth: %d\nFlags: %d\nWidth: %d\nHeight: %d\nStride: %d\nByteSize: %d\n", pcx->BitDepth, pcx->Flags, pcx->Width, pcx->Height, pcx->Stride, pcx->ByteSize);
	//WaitForKey();
	//MISC->SetBitmapMode256(SMODE_320 | SMODE_240);
	//MISC->DmaCopy((void*)0x0E000000, (int8_t*)(pcx + pcx->ImageOffset), pcx->ByteSize, DMA_INT);
	//MISC->DmaCopy((void*)0x0E100000, (int8_t*)(pcx + pcx->ColorOffset), 512, DMA_SHORT);
	DRAW->DisplayPicture(pcx);
	while(1);
}
*/

/*
static const uint16_t hdma1[] =
{
	0x0006,0x0006,0x0006,0x0006,0x0007,0x0007,0x0007,0x0007,
	0x0408,0x0408,0x0408,0x0408,0x0409,0x0409,0x0409,0x0409,
	0x080A,0x080A,0x080A,0x080A,0x080B,0x080B,0x080B,0x080C,
	0x0C0C,0x0C0C,0x0C0C,0x0C0D,0x0C0D,0x0C0D,0x0C0D,0x0C0E,
	0x100E,0x100E,0x100E,0x100F,0x100F,0x100F,0x100F,0x1010,
	0x1410,0x1410,0x1410,0x1411,0x1411,0x1411,0x1411,0x1412,
	0x1812,0x1812,0x1812,0x1813,0x1813,0x1813,0x1813,0x1C14,
	0x1C14,0x1C14,0x1C15,0x1C15,0x1C15,0x1C15,0x1C16,0x2016,
	0x2016,0x2016,0x2017,0x2017,0x2017,0x2017,0x2018,0x2418,
	0x2418,0x2418,0x2419,0x2419,0x2419,0x2419,0x241A,0x281A,
	0x281A,0x281A,0x281B,0x281B,0x281B,0x281B,0x281C,0x2C1C,
	0x2C1C,0x2C1C,0x2C1D,0x2C1D,0x2C1D,0x2C1D,0x2C1E,0x301E,
	0x301E,0x301E,0x301E,0x301E,0x301D,0x301D,0x301D,0x301D,
	0x301D,0x301C,0x301C,0x301C,0x301C,0x301B,0x301B,0x301B,
	0x301B,0x301A,0x301A,0x301A,0x301A,0x301A,0x3019,0x3019,
	0x3019,0x3019,0x3018,0x3018,0x3018,0x3018,0x3017,0x3017,
	0x3017,0x3017,0x3017,0x3016,0x3016,0x3016,0x3016,0x3015,
	0x3015,0x3015,0x3015,0x3014,0x3014,0x3014,0x3014,0x3014,
	0x3013,0x3013,0x3013,0x3013,0x3012,0x3012,0x3012,0x3012,
	0x3011,0x3011,0x3011,0x3011,0x3010,0x3010,0x3010,0x3010,
	0x3010,0x300F,0x300F,0x300F,0x300F,0x300E,0x300E,0x300E,
	0x300E,0x300D,0x300D,0x300D,0x300D,0x300D,0x300C,0x300C,
	0x300C,0x300C,0x300C,0x300C,0x300C,0x302B,0x302B,0x342B,
	0x342B,0x342B,0x344B,0x344B,0x344B,0x344B,0x344B,0x346B,
	0x386B,0x386B,0x386A,0x386A,0x388A,0x388A,0x388A,0x388A,
	0x388A,0x3CAA,0x3CAA,0x3CAA,0x3CAA,0x3CCA,0x3CC9,0x3CC9,
	0x3CC9,0x3CC9,0x40E9,0x40E9,0x40E9,0x40E9,0x40E9,0x4109,
	0x4109,0x4109,0x4509,0x4508,0x4528,0x4528,0x4528,0x4528,
	0x4528,0x4548,0x4548,0x4948,0x4948,0x4948,0x4968,0x4967,
	0x4967,0x4967,0x4987,0x4987,0x4D87,0x4D87,0x4D87,0x4DA7,
};

static const uint16_t hdma2[] =
{
	0x00C6,0x016B,0x0231,0x02F7,0x039C,0x035A,0x0318,0x02D6,
	0x0294,0x0252,0x0210,0x01CE,0x018C,0x014A,0x0108,0x00C6,
};

static const uint16_t sinlut[] =
{
	0x0000,0x0032,0x0064,0x0096,0x00C8,0x00FB,0x012D,0x015F,
	0x0191,0x01C3,0x01F5,0x0227,0x0259,0x028A,0x02BC,0x02ED,
	0x031F,0x0350,0x0381,0x03B2,0x03E3,0x0413,0x0444,0x0474,
	0x04A5,0x04D5,0x0504,0x0534,0x0563,0x0593,0x05C2,0x05F0,
	0x061F,0x064D,0x067B,0x06A9,0x06D7,0x0704,0x0731,0x075E,
	0x078A,0x07B7,0x07E2,0x080E,0x0839,0x0864,0x088F,0x08B9,
	0x08E3,0x090D,0x0936,0x095F,0x0987,0x09B0,0x09D7,0x09FF,
	0x0A26,0x0A4D,0x0A73,0x0A99,0x0ABE,0x0AE3,0x0B08,0x0B2C,
	0x0B50,0x0B73,0x0B96,0x0BB8,0x0BDA,0x0BFC,0x0C1D,0x0C3E,
	0x0C5E,0x0C7D,0x0C9D,0x0CBB,0x0CD9,0x0CF7,0x0D14,0x0D31,
	0x0D4D,0x0D69,0x0D84,0x0D9F,0x0DB9,0x0DD2,0x0DEB,0x0E04,
	0x0E1C,0x0E33,0x0E4A,0x0E60,0x0E76,0x0E8B,0x0EA0,0x0EB4,
	0x0EC8,0x0EDB,0x0EED,0x0EFF,0x0F10,0x0F21,0x0F31,0x0F40,
	0x0F4F,0x0F5D,0x0F6B,0x0F78,0x0F85,0x0F91,0x0F9C,0x0FA7,
	0x0FB1,0x0FBA,0x0FC3,0x0FCB,0x0FD3,0x0FDA,0x0FE1,0x0FE7,
	0x0FEC,0x0FF0,0x0FF4,0x0FF8,0x0FFB,0x0FFD,0x0FFE,0x0FFF,
	0x1000,0x0FFF,0x0FFE,0x0FFD,0x0FFB,0x0FF8,0x0FF4,0x0FF0,
	0x0FEC,0x0FE7,0x0FE1,0x0FDA,0x0FD3,0x0FCB,0x0FC3,0x0FBA,
	0x0FB1,0x0FA7,0x0F9C,0x0F91,0x0F85,0x0F78,0x0F6B,0x0F5D,
	0x0F4F,0x0F40,0x0F31,0x0F21,0x0F10,0x0EFF,0x0EED,0x0EDB,
	0x0EC8,0x0EB4,0x0EA0,0x0E8B,0x0E76,0x0E60,0x0E4A,0x0E33,
	0x0E1C,0x0E04,0x0DEB,0x0DD2,0x0DB9,0x0D9F,0x0D84,0x0D69,
	0x0D4D,0x0D31,0x0D14,0x0CF7,0x0CD9,0x0CBB,0x0C9D,0x0C7D,
	0x0C5E,0x0C3E,0x0C1D,0x0BFC,0x0BDA,0x0BB8,0x0B96,0x0B73,
	0x0B50,0x0B2C,0x0B08,0x0AE3,0x0ABE,0x0A99,0x0A73,0x0A4D,
	0x0A26,0x09FF,0x09D7,0x09B0,0x0987,0x095F,0x0936,0x090D,
	0x08E3,0x08B9,0x088F,0x0864,0x0839,0x080E,0x07E2,0x07B7,
	0x078A,0x075E,0x0731,0x0704,0x06D7,0x06A9,0x067B,0x064D,
	0x061F,0x05F0,0x05C2,0x0593,0x0563,0x0534,0x0504,0x04D5,
	0x04A5,0x0474,0x0444,0x0413,0x03E3,0x03B2,0x0381,0x0350,
	0x031F,0x02ED,0x02BC,0x028A,0x0259,0x0227,0x01F5,0x01C3,
	0x0191,0x015F,0x012D,0x00FB,0x00C8,0x0096,0x0064,0x0032,
	0x0000,0xFFCE,0xFF9C,0xFF6A,0xFF38,0xFF05,0xFED3,0xFEA1,
	0xFE6F,0xFE3D,0xFE0B,0xFDD9,0xFDA7,0xFD76,0xFD44,0xFD13,
	0xFCE1,0xFCB0,0xFC7F,0xFC4E,0xFC1D,0xFBED,0xFBBC,0xFB8C,
	0xFB5B,0xFB2B,0xFAFC,0xFACC,0xFA9D,0xFA6D,0xFA3E,0xFA10,
	0xF9E1,0xF9B3,0xF985,0xF957,0xF929,0xF8FC,0xF8CF,0xF8A2,
	0xF876,0xF849,0xF81E,0xF7F2,0xF7C7,0xF79C,0xF771,0xF747,
	0xF71D,0xF6F3,0xF6CA,0xF6A1,0xF679,0xF650,0xF629,0xF601,
	0xF5DA,0xF5B3,0xF58D,0xF567,0xF542,0xF51D,0xF4F8,0xF4D4,
	0xF4B0,0xF48D,0xF46A,0xF448,0xF426,0xF404,0xF3E3,0xF3C2,
	0xF3A2,0xF383,0xF363,0xF345,0xF327,0xF309,0xF2EC,0xF2CF,
	0xF2B3,0xF297,0xF27C,0xF261,0xF247,0xF22E,0xF215,0xF1FC,
	0xF1E4,0xF1CD,0xF1B6,0xF1A0,0xF18A,0xF175,0xF160,0xF14C,
	0xF138,0xF125,0xF113,0xF101,0xF0F0,0xF0DF,0xF0CF,0xF0C0,
	0xF0B1,0xF0A3,0xF095,0xF088,0xF07B,0xF06F,0xF064,0xF059,
	0xF04F,0xF046,0xF03D,0xF035,0xF02D,0xF026,0xF01F,0xF019,
	0xF014,0xF010,0xF00C,0xF008,0xF005,0xF003,0xF002,0xF001,
	0xF000,0xF001,0xF002,0xF003,0xF005,0xF008,0xF00C,0xF010,
	0xF014,0xF019,0xF01F,0xF026,0xF02D,0xF035,0xF03D,0xF046,
	0xF04F,0xF059,0xF064,0xF06F,0xF07B,0xF088,0xF095,0xF0A3,
	0xF0B1,0xF0C0,0xF0CF,0xF0DF,0xF0F0,0xF101,0xF113,0xF125,
	0xF138,0xF14C,0xF160,0xF175,0xF18A,0xF1A0,0xF1B6,0xF1CD,
	0xF1E4,0xF1FC,0xF215,0xF22E,0xF247,0xF261,0xF27C,0xF297,
	0xF2B3,0xF2CF,0xF2EC,0xF309,0xF327,0xF345,0xF363,0xF383,
	0xF3A2,0xF3C2,0xF3E3,0xF404,0xF426,0xF448,0xF46A,0xF48D,
	0xF4B0,0xF4D4,0xF4F8,0xF51D,0xF542,0xF567,0xF58D,0xF5B3,
	0xF5DA,0xF601,0xF629,0xF650,0xF679,0xF6A1,0xF6CA,0xF6F3,
	0xF71D,0xF747,0xF771,0xF79C,0xF7C7,0xF7F2,0xF81E,0xF849,
	0xF876,0xF8A2,0xF8CF,0xF8FC,0xF929,0xF957,0xF985,0xF9B3,
	0xF9E1,0xFA10,0xFA3E,0xFA6D,0xFA9D,0xFACC,0xFAFC,0xFB2B,
	0xFB5B,0xFB8C,0xFBBC,0xFBED,0xFC1D,0xFC4E,0xFC7F,0xFCB0,
	0xFCE1,0xFD13,0xFD44,0xFD76,0xFDA7,0xFDD9,0xFE0B,0xFE3D,
	0xFE6F,0xFEA1,0xFED3,0xFF05,0xFF38,0xFF6A,0xFF9C,0xFFCE,
	0x0000,0x0032
};

#define REG_HDMACONTROL	((uint32_t*)(MEM_IO+0x0080))
#define REG_HDMASOURCE	((uint32_t*)(MEM_IO+0x00A0))
#define REG_HDMATARGET	((uint32_t*)(MEM_IO+0x00C0))

#define DMA_ENABLE 0x01
#define HDMA_DOUBLE 0x80

int32_t main()
{
	interface = (IBios*)(0x01000000);
	MISC->SetBitmapMode16(SMODE_320 | SMODE_240);
	TEXT->ClearScreen();

	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (sizeof(hdma1) << 20);

	REG_HDMASOURCE[1] = (int32_t)hdma2;
	REG_HDMATARGET[1] = (int32_t)PALETTE;
	REG_HDMASOURCE[2] = (int32_t)hdma2;
	REG_HDMATARGET[2] = (int32_t)PALETTE;

	for (int32_t i = 0; i < 62; i++)
		DRAW->DrawChar('A' + i, 4 + (i * 2),  4 + (i * 2), (i % 15) + 1);

	DRAW->DrawString("Hello, world!_", 33, 5, 8);
	DRAW->DrawString("Hello, world!_", 32, 4, 15);

	int32_t i;

	while(1)
	{
		vbl();
		i++;
		if (i == 514) i = 0;
		REG_HDMACONTROL[1] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | ( ((sinlut[(i<<1)%512] >> 5) + 160) << 8) | (sizeof(hdma2) << 20);
		REG_HDMACONTROL[2] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | ( ((sinlut[(i<<2)%512] >> 5) + 160) << 8) | (sizeof(hdma2) << 20);
	}
}
*/
/*
uint8_t* midiFile;
uint8_t* midiTrackOneStart;
uint8_t* midiTrackOnePos;
uint8_t* midiTrackOneEnd;
//int32 midiTrackOneDelay;
int32_t midiTempo;
int32_t midiPPQN;
int32_t midiPSecPerTick;
int32_t midiLastEventTime, midiNextEventTime, midiEventTimeNow;
uint8_t midiRunningStatus;
int32_t playing;
int32_t midiRow;

uint8_t GetMIDIByte(int32_t track)
{
	uint8_t ret = *midiTrackOnePos;
	//dpf("[GetMIDIByte: %x]", ret);
	midiTrackOnePos++;
	return ret;
}

uint32_t GetDelta()
{
	uint8_t val;
	uint8_t c;
	if ((val = GetMIDIByte(0)) & 0x80)
	{
		val &= 0x7F;
		do
		{
			val = (val << 7) + ((c = GetMIDIByte(0)) & 0x7F);
		} while (c & 0x80);
	}
	//dpf("[delta= %d]", val);
	return val;
}

void UpdateMIDI()
{
	if (!playing)
		return;
	if (midiTrackOnePos >= midiTrackOneEnd)
	{
		playing = 0;
		printf("Finished.");
		return;
	}

	interface->textLibrary->SetCursorPosition(0, 7);
	printf("%p / %p\n", midiTrackOnePos - midiFile, midiTrackOneEnd - midiFile);
	printf("%8d / %-8d\n", midiNextEventTime, midiLastEventTime);

	if (midiNextEventTime > 0)
	{
		midiNextEventTime -= midiPSecPerTick;
		return;
	}

	while (1)
	{
		interface->textLibrary->SetCursorPosition(0, midiRow);
		//printf("!");
		//midiTrackOneDelay = GetDelta(midiTrackOnePos) * 10;
		int32_t delta = GetDelta();
		midiLastEventTime = delta;
		midiNextEventTime = delta;
		printf("%8d ", delta);

		//printf("%8d ", midiTrackOneDelay);
		printf("%p ", midiTrackOnePos - midiFile);
		uint8_t command = GetMIDIByte(0);
		printf("%02x ", command);
		//printf("[Command: %x]", command);
		switch (command)
		{
			case 0xFF:
			{
				command = GetMIDIByte(0);
				printf("%02x ", command);
				switch (command)
				{
					case 0x02:
						//printf("copyright name\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x03:
						//printf("track name\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x20:
						//printf("channel prefix\n");
						midiTrackOnePos+=2;
						break;
					case 0x2F:
						printf("EOT");
						playing = 0;
						break;
					case 0x51:
						//printf("tempo\n");
						interface->textLibrary->SetCursorPosition(0, 14);
						GetMIDIByte(0);
						midiTempo = (GetMIDIByte(0) << 16) | (GetMIDIByte(0) << 8) | GetMIDIByte(0);
						printf("tempo %d\n", midiTempo);
						midiPSecPerTick = (midiTempo + (midiPPQN >> 2)) / midiPPQN;
						printf("pSecPerTick %d", midiPSecPerTick);
						//midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x58:
						//printf("time signature\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x59:
						//printf("key signature\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					case 0x7F:
						//printf("sequencer-specific\n");
						midiTrackOnePos+=GetMIDIByte(0);
						break;
					default:
						printf("*unhandled FF: %02x*\n", command);
						playing = 0;
						break;
				}
				break;
			}
			default:
			{
				if (command < 0x80 && midiRunningStatus > 0)
					command = midiRunningStatus;
				if (command >= 0x80 && command <= 0x8F)
				{
					//Key On
					int32_t channel = command & 0x0F;
					int32_t pitch = GetMIDIByte(0);
					int32_t velocity = GetMIDIByte(0);
					printf("KOF %3d %3d %3d", channel, pitch, velocity);
					MIDI_KEYOFF(channel, pitch, velocity);
				}
				else if (command >= 0x90 && command <= 0x9F)
				{
					//Key On
					int32_t channel = command & 0x0F;
					int32_t pitch = GetMIDIByte(0);
					int32_t velocity = GetMIDIByte(0);
					printf("KON %3d %3d %3d", channel, pitch, velocity);
					MIDI_KEYON(channel, pitch, velocity);
				}
				else if (command >= 0xB0 && command <= 0xBF)
				{
					//Control Change
					int32_t channel = command & 0x0F;
					int32_t control = GetMIDIByte(0);
					int32_t value = GetMIDIByte(0);
					printf("CTL %3d %3d %3d", channel, control, value);
					MIDI_CONTROL(channel, control, value);
				}
				else if (command >= 0xC0 && command <= 0xCF)
				{
					//Program Change
					int32_t channel = command & 0x0F;
					int32_t voice = GetMIDIByte(0);
					printf("PRG %3d %3d %3d", channel, voice);
					MIDI_PROGRAM(channel, voice);
				}
				else if (command >= 0xE0 && command <= 0xEF)
				{
					//Pitch Bend
					int32_t channel = command & 0x0F;
					int32_t control = GetMIDIByte(0);
					int32_t value = GetMIDIByte(0);
					printf("PIT %3d %3d %3d", channel, control, value);
					REG_MIDIOUT = (0xE0 | channel) | (control << 8) | (value << 16);
				}
				else
				{
					printf("*unhandled: %02x*\n", command);
					playing = 0;
					break;
				}
				midiRunningStatus = command;
			}
		}
		printf("              \n");
		midiRow++;
		if (delta) break;
		printf("...");
	}
	//interface->textLibrary->SetCursorPosition(0, 0);
	//printf("%d\n", midiTrackOneDelay);
	//midiTrackOneDelay -= (deltaTicks);
}

int32_t main()
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetCursorPosition(0, 0);
	TEXT->SetTextColor(0, 7);

	FILEINFO nfo;
	FILE file;
	int32_t ret = DISK->FileStat("FOO.MID", &nfo);
	if (ret != 0)
	{
		printf("Something fucked up: %d\n", ret);
		while(1);
	}
	midiFile = (uint8_t*)malloc(nfo.fsize);
	printf("MIDI File allocated at %p\n", midiFile);
	DISK->OpenFile(&file, "FOO.MID", FA_READ);
	DISK->ReadFile(&file, (void*)midiFile, nfo.fsize);
	DISK->CloseFile(&file);

	midiTempo = 120;
	midiPPQN = 96;

	midiRow = 10;

	//deltaTicks = *(uint16*)(midiFile + 0xC);
	//printf("deltaTicks is %d/%#x\n", deltaTicks, deltaTicks);
	//midiPPQN = *(uint16_t*)(midiFile + 0xC);
	midiTrackOneStart = midiFile + 0x16;
	printf("Track one starts at %p\n", midiTrackOneStart - midiFile);
	uint32_t len = *(uint32_t*)(midiFile + 0x12);
	midiTrackOneEnd = midiTrackOneStart + len;
	printf("Track one is %#x long, ends at %p\n", len, midiTrackOneEnd);
	midiTrackOnePos = midiTrackOneStart;
	//midiTrackOneDelay = 0;
	midiEventTimeNow = midiLastEventTime = midiNextEventTime = 0;
	midiRunningStatus = 0;

	midiPSecPerTick = 60000 / (midiTempo * midiPPQN);
	printf("PSecPerTick is 60000 / (%d * %d) = %d\n", midiTempo, midiPPQN, midiPSecPerTick);

	playing = 1;

	while(playing)
	//for (int32_t i = 0; i < 4; i++)
	{
		UpdateMIDI();
		vbl();
	}

	while(1);
}
*/

/*
#define DISKRAM ((unsigned char*)0x0D7FFE00)
#define REG_DISKSECTOR	*(uint16_t*)(0x0D800030)
#define REG_DISKCONTROL	*(volatile unsigned char*)(0x0D800032)
#define DCTL_PRESENT 1
#define DCTL_ERROR 2
#define DCTL_READNOW 4
#define DCTL_WRITENOW 8

const char format_0[] =
{
	0xEB,0x3C,0x90,0x4D,0x53,0x44,0x4F,0x53,0x35,0x2E,0x30,0x00,0x02,0x01,0x01,0x00,
	0x02,0xE0,0x00,0x40,0x0B,0xF0,0x09,0x00,0x12,0x00,0x02,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x29,0x38,0x25,0xA8,0x6E,0x4E,0x4F,0x20,0x4E,0x41,
	0x4D,0x45,0x20,0x20,0x20,0x20,0x46,0x41,0x54,0x31,0x32,0x20,0x20,0x20,0x33,0xC9,
	0x8E,0xD1,0xBC,0xF0,0x7B,0x8E,0xD9,0xB8,0x00,0x20,0x8E,0xC0,0xFC,0xBD,0x00,0x7C,
	0x38,0x4E,0x24,0x7D,0x24,0x8B,0xC1,0x99,0xE8,0x3C,0x01,0x72,0x1C,0x83,0xEB,0x3A,
	0x66,0xA1,0x1C,0x7C,0x26,0x66,0x3B,0x07,0x26,0x8A,0x57,0xFC,0x75,0x06,0x80,0xCA,
	0x02,0x88,0x56,0x02,0x80,0xC3,0x10,0x73,0xEB,0x33,0xC9,0x8A,0x46,0x10,0x98,0xF7,
	0x66,0x16,0x03,0x46,0x1C,0x13,0x56,0x1E,0x03,0x46,0x0E,0x13,0xD1,0x8B,0x76,0x11,
	0x60,0x89,0x46,0xFC,0x89,0x56,0xFE,0xB8,0x20,0x00,0xF7,0xE6,0x8B,0x5E,0x0B,0x03,
	0xC3,0x48,0xF7,0xF3,0x01,0x46,0xFC,0x11,0x4E,0xFE,0x61,0xBF,0x00,0x00,0xE8,0xE6,
	0x00,0x72,0x39,0x26,0x38,0x2D,0x74,0x17,0x60,0xB1,0x0B,0xBE,0xA1,0x7D,0xF3,0xA6,
	0x61,0x74,0x32,0x4E,0x74,0x09,0x83,0xC7,0x20,0x3B,0xFB,0x72,0xE6,0xEB,0xDC,0xA0,
	0xFB,0x7D,0xB4,0x7D,0x8B,0xF0,0xAC,0x98,0x40,0x74,0x0C,0x48,0x74,0x13,0xB4,0x0E,
	0xBB,0x07,0x00,0xCD,0x10,0xEB,0xEF,0xA0,0xFD,0x7D,0xEB,0xE6,0xA0,0xFC,0x7D,0xEB,
	0xE1,0xCD,0x16,0xCD,0x19,0x26,0x8B,0x55,0x1A,0x52,0xB0,0x01,0xBB,0x00,0x00,0xE8,
	0x3B,0x00,0x72,0xE8,0x5B,0x8A,0x56,0x24,0xBE,0x0B,0x7C,0x8B,0xFC,0xC7,0x46,0xF0,
	0x3D,0x7D,0xC7,0x46,0xF4,0x29,0x7D,0x8C,0xD9,0x89,0x4E,0xF2,0x89,0x4E,0xF6,0xC6,
	0x06,0x96,0x7D,0xCB,0xEA,0x03,0x00,0x00,0x20,0x0F,0xB6,0xC8,0x66,0x8B,0x46,0xF8,
	0x66,0x03,0x46,0x1C,0x66,0x8B,0xD0,0x66,0xC1,0xEA,0x10,0xEB,0x5E,0x0F,0xB6,0xC8,
	0x4A,0x4A,0x8A,0x46,0x0D,0x32,0xE4,0xF7,0xE2,0x03,0x46,0xFC,0x13,0x56,0xFE,0xEB,
	0x4A,0x52,0x50,0x06,0x53,0x6A,0x01,0x6A,0x10,0x91,0x8B,0x46,0x18,0x96,0x92,0x33,
	0xD2,0xF7,0xF6,0x91,0xF7,0xF6,0x42,0x87,0xCA,0xF7,0x76,0x1A,0x8A,0xF2,0x8A,0xE8,
	0xC0,0xCC,0x02,0x0A,0xCC,0xB8,0x01,0x02,0x80,0x7E,0x02,0x0E,0x75,0x04,0xB4,0x42,
	0x8B,0xF4,0x8A,0x56,0x24,0xCD,0x13,0x61,0x61,0x72,0x0B,0x40,0x75,0x01,0x42,0x03,
	0x5E,0x0B,0x49,0x75,0x06,0xF8,0xC3,0x41,0xBB,0x00,0x00,0x60,0x66,0x6A,0x00,0xEB,
	0xB0,0x4E,0x54,0x4C,0x44,0x52,0x20,0x20,0x20,0x20,0x20,0x20,0x0D,0x0A,0x52,0x65,
	0x6D,0x6F,0x76,0x65,0x20,0x64,0x69,0x73,0x6B,0x73,0x20,0x6F,0x72,0x20,0x6F,0x74,
	0x68,0x65,0x72,0x20,0x6D,0x65,0x64,0x69,0x61,0x2E,0xFF,0x0D,0x0A,0x44,0x69,0x73,
	0x6B,0x20,0x65,0x72,0x72,0x6F,0x72,0xFF,0x0D,0x0A,0x50,0x72,0x65,0x73,0x73,0x20,
	0x61,0x6E,0x79,0x20,0x6B,0x65,0x79,0x20,0x74,0x6F,0x20,0x72,0x65,0x73,0x74,0x61,
	0x72,0x74,0x0D,0x0A,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xAC,0xCB,0xD8,0x55,0xAA
};

void FormatDisk()
{
	//Set up sector zero
	REG_DMASOURCE = (int32_t)format_0;
	REG_DMATARGET = (int32_t)DISKRAM;
	REG_DMALENGTH = 512;
	REG_DMACONTROL = 0x07;

	//Write sector zero
	REG_DISKSECTOR = 0;
	REG_DISKCONTROL = DCTL_WRITENOW;

	//Set up blankness
	REG_DMASOURCE = 0;
	REG_DMATARGET = (int32_t)DISKRAM;
	REG_DMALENGTH = 512;
	REG_DMACONTROL = 0x0D;

	//Write all blank sectors
	for (int32_t i = 1; i < 256; i++)
	{
		REG_DISKSECTOR = i;
		REG_DISKCONTROL = DCTL_WRITENOW;
	}

	//Set up blank chains
	*(int32_t*)DISKRAM = 0xF0FFFF00;

	//Write blank chains
	REG_DISKSECTOR = 1;
	REG_DISKCONTROL = DCTL_WRITENOW;
	REG_DISKSECTOR = 10;
	REG_DISKCONTROL = DCTL_WRITENOW;

	//And we're done!
}

int32_t main()
{
	interface = (IBios*)(0x01000000);
	FormatDisk();
	while(1);
}
*/

/*
void DrawChar8_320(char c, int32_t x, int32_t y, int32_t color)
{
	char* glyph = (char*)0x0E100A00 + (c * 8);
	char* target = (char*)0x0E000000 + (y * 320) + x;
	for (int32_t line = 0; line < 8; line++)
	{
		for (int32_t bit = 0; bit < 8; bit++)
		{
			int32_t pixel = (*glyph >> bit) & 1;
			if (pixel == 0)
				continue;
			target[bit] = color;
		}
		glyph++;
		target += 320;
	}
}

void DrawChar8_640(char c, int32_t x, int32_t y, int32_t color)
{
	char* glyph = (char*)0x0E100A00 + (c * 8);
	char* target = (char*)0x0E000000 + (y * 640) + x;
	for (int32_t line = 0; line < 8; line++)
	{
		for (int32_t bit = 0; bit < 8; bit++)
		{
			int32_t pixel = (*glyph >> bit) & 1;
			if (pixel == 0)
				continue;
			target[bit] = color;
		}
		glyph++;
		target += 640;
	}
}

void DrawChar4_320(char c, int32_t x, int32_t y, int32_t color)
{
	char* glyph = (char*)0x0E100A00 + (c * 8);
	char* target = (char*)0x0E000000 + (y * 160) + (x / 2);
	if (x % 2 == 0)
	{
		for (int32_t line = 0; line < 8; line++)
		{
			char g = *glyph++;
			for (int32_t bit = 0; bit < 8; bit += 2)
			{
				int32_t p = g >> bit;
				if (p & 1)
					*target = (*target & 0x0F) | (color << 4);
				if (p & 2)
					*target = (*target & 0xF0) | color;
				target++;
			}
			target += 160 - 4;
		}
	}
	else
	{
		for (int32_t line = 0; line < 8; line++)
		{
			char g = *glyph++;
			if (g & 1)
				*target = (*target & 0xF0) | color;
			for (int32_t bit = 1; bit < 7; bit += 2)
			{
				target++;
				int32_t p = g >> bit;
				if (p & 1)
					*target = (*target & 0x0F) | (color << 4);
				if (p & 2)
					*target = (*target & 0xF0) | color;
			}
			target++;
			if ((g >> 7) & 1)
				*target = (*target & 0x0F) | (color << 4);
			target += 160 - 4;
		}
	}
}

void DrawString(const char* str, int32_t x, int32_t y, int32_t color, void(*renderer)(char,int32_t,int32_t,int32_t))
{
	while(*str)
	{
		renderer(*str++, x, y, color);
		x += 8;
	}
}

int32_t main()
{
	interface = (IBios*)(0x01000000);
	REG_SCREENMODE = SMODE_BMP1 | SMODE_320 | SMODE_240;

	for (int32_t i = 0; i < 62; i++)
		DrawChar4_320('A' + i, 4 + (i * 2),  4 + (i * 2), (i % 15) + 1);

	DrawString("Hello, world!_", 33, 5, 8, DrawChar4_320);
	DrawString("Hello, world!_", 32, 4, 15, DrawChar4_320);

	while(1);
}
*/

/*
typedef struct malloc_block_meta {
	int32_t size;
	struct malloc_block_meta* next;
	int32_t free;
} malloc_block_meta;
extern void* malloc_global_base;

void printheap()
{
	malloc_block_meta* b = malloc_global_base;
	for(;b;b = b->next)
	{
		interface->textLibrary->SetTextColor(0, b->free ? 2 : 10);
		printf("[%d,%#x]", b->size, b->size);
	}
	interface->textLibrary->SetTextColor(0, 7);
	printf("\n");
}

int32_t main()
{
	interface = (IBios*)(0x01000000);
	interface->textLibrary->SetTextMode80x30();
	REG_SCREENMODE = SMODE_BOLD | SMODE_240;
	interface->textLibrary->SetCursorPosition(0, 0);
	interface->textLibrary->SetTextColor(0, 7);

	printf("Okay... about to malloc 1024 bytes.\n");
	char* kilo = malloc(1024);
	printf("Returned value is %p.\n", kilo);
	printheap();
	printf("Freeing that part...\n");
	free(kilo);
	printheap();
	printf("...and mallocing another 2048 bytes.\n");
	char* kilo2 = malloc(2048);
	printf("Returned value is %p.\n", kilo2);
	printheap();
	printf("Malloc another 1024 bytes.\n");
	kilo = malloc(1024);
	printf("Returned value is %p.\n", kilo);
	printheap();
	printf("Freeing that part...\n");
	free(kilo);
	printheap();
	printf("...and mallocing 512 bytes.\n");
	char* half = malloc(512);
	printf("Returned value is %p.\n", half);
	printheap();
	printf("The big one: 640*480 bytes.\n");
	char* screenfulofpixels = malloc(640*480);
	printf("Returned value is %p.\n", screenfulofpixels);
	printheap();

	while(1);
}
*/
