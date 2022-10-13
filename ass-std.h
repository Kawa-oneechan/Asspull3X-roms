#pragma once

#include "ass.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STDIN  ((FILE*)-1)
#define STDOUT ((FILE*)-2)
#define EOF (-1)
#define MIN_GETLINE_CHUNK 64

extern int fputc(int c, FILE* file);
extern int fputs(const char* s, FILE* file);
extern int fgetc(FILE* file);
extern int getdelim(char** linePtr, int* n, char delim, FILE* file);
extern char* fgets(char* s, int n, FILE* file);
extern char* gets_s(char* s, int n);
extern size_t fread(void* data, int size, size_t count, FILE* file);
extern size_t fwrite(void* data, int size, size_t count, FILE* file);
extern FILE* fopen(const char* filename, const char* opentype);
extern int fclose(FILE* file);

#define putc(c, file) fputc(c, file)
#define putchar(c) fputc(c, STDOUT)
#define puts(s) TEXT->Write(s); TEXT->WriteChar('\n');
#define getc(file) fgetc(file)
#define getchar() fgetc(STDIN)
#define getline(l,n,f) getdelim(l,n,'\n',f)
//#define gets_s(s,n) fgets(s,n,STDIN)
#define feof(f) DISK->FileEnd(f)
#define ftell(f) DISK->FilePosition(f)
#define fseek(f,o,w) DISK->SeekFile(f,o,w)
#define rewind(f) DISK->SeekFile(f, 0, SEEK_SET)
#define sprintf(b,f,rest...) TEXT->Format(b,f, ## rest)

extern char * strtok_r (char *newstring, const char *delimiters, char **save_ptr);

#ifdef __cplusplus
}
#endif
