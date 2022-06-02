#include "ass-std.h"
extern IBios* interface;

static const char sctoasc[] = {
//Unshifted
//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	0,   '\e','1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b','\t',// 0x00
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',0,   'a', 's', // 0x10
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`', 0,   '\\','z', 'x', 'c', 'v', // 0x20
	'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   // 0x30
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x50
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x80
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '\n',0,   0,   0,   // 0x90
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xa0
	0,   0,   0,   0,   0,   '/', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xb0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xc0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xd0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xe0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xf0
//Shifted
//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	0,   '\e','!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b','\t',// 0x00
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',0,   'A', 'S', // 0x10
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"','~', 0,   '|', 'Z', 'X', 'C', 'V', // 0x20
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   // 0x30
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40
	'2', '3', '0', '.', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x50
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x80
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '\n',0,   0,   0,   // 0x90
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xa0
	0,   0,   0,   0,   0,   '/', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xb0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xc0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xd0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xe0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xf0
};

char __fgetc_echo = 1;

int fputc(int c, FILE* file)
{
	if (file == STDOUT)
	{
		TEXT->WriteChar((char)c);
		return c;
	}
	uint8_t _c = (uint8_t)c;
	DISK->WriteFile(file, &_c, 1);
	return c;
}

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

int fgetc(FILE* file)
{
	if (file != STDIN)
	{
		if (DISK->FileEnd(file)) return EOF;
		char c;
		DISK->ReadFile(file, &c, 1);
		return c;
	}
	uint16_t key = 0;
	while (1)
	{
		if (INP_KEYSHIFT & 2)
		{
			int altcode = 0, altnum = 0;
			while (INP_KEYSHIFT & 2)
			{
				key = INP_KEYIN;
				if (key >= 0x2 && key < 0xB)
				{
					altcode *= 10;
					altcode += sctoasc[key] - '0';
					altnum++;
					if (altnum == 3)
						break;
				}
			}
			if (altnum == 3)
			{
				if (file == STDIN && __fgetc_echo) TEXT->WriteChar(altcode);
				return altcode;
			}
		}
		key = INP_KEYIN & 0xFF;
		if (key > 0)
			break;
	}
	while (1)
	{
		if ((INP_KEYIN & 0xFF) == 0)
			break;
	}
	if (INP_KEYSHIFT & 1)
		key += 256;
	char asc = sctoasc[key];
	if (file == STDIN && __fgetc_echo) TEXT->WriteChar(asc);
	return asc;
}

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

char* fgets(char* s, int n, FILE* file)
{
	char* _s = s;
	int c = 0;
	char echo = __fgetc_echo;
	__fgetc_echo = 0;
	while(--n)
	{
		c = fgetc(file);
		if (c == '\x1B' && file == STDIN)
		{
			//eat the esc
			n++;
			continue;
		}
		if (((file == STDIN && c == '\n') || (file != STDIN && c == 0)) || c == EOF)
			break;
		if (c == '\b' && file == STDIN)
		{
			if (_s > s)
			{
				putchar('\b');
				*_s-- = 0;
				n++;
			}
			continue;
		}
		if (file == STDIN) putchar((char)c);
		*_s++ = (char)c;
	}
	if (c == EOF && _s == s)
		return 0;
	*_s = 0;
	if (file == STDIN) putchar('\n');
	__fgetc_echo = echo;
	return s;
}

size_t fread(void* data, int size, size_t count, FILE* file)
{
	if (size == 0 || count == 0) return 0;
	size_t ret = 0;
	for (; ret < count; ret++)
	{
		if (DISK->ReadFile(file, data, size) < 0) break;
		data += size;
	}
	return ret;
}

size_t fwrite(void* data, int size, size_t count, FILE* file)
{
	if (size == 0 || count == 0) return 0;
	size_t ret = 0;
	for (; ret < count; ret++)
	{
		if (DISK->WriteFile(file, data, size) < 0) break;
		data += size;
	}
	return ret;
}

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

