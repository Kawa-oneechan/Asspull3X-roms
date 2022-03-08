#include "ass-std.h"
extern IBios* interface;

static const char sctoasc[256] = {
	0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
	'q','w','e','r','t','y','u','i','o','p','[',']',0,0,'a','s',
	'd','f','g','h','j','k','l',';',39,'`',0,92,'z','x','c','v',
	'b','n','m',',','.','/',0,'*',0,32,0,0,0,0,0,0,
	0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
	'2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};
static const char sctoasc_shift[256] = {
//Shifted
	0,0,'!','@','#','$','%','^','&','*','(',')','_','+',0,0,
	'Q','W','E','R','T','Y','U','I','O','P','{','}',0,0,'A','S',
	'D','F','G','H','J','K','L',':',34,'~',0,'|','Z','X','C','V',
	'B','N','M','<','>','?',0,'*',0,32,0,0,0,0,0,0,
	0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
	'2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

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
		//char buffer[1];
		//DISK->ReadFile(stream, buffer, 1);
		//return buffer[0];
	}
	uint16_t key = 0;
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
	char asc = sctoasc[key & 0xFF];
	if ((key & 0x100) == 0x100) asc = sctoasc_shift[key & 0xFF];
	if (file == STDIN) TEXT->WriteChar(asc);
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

