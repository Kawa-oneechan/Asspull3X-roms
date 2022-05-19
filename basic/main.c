#ifdef WIN32
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#else
#include "../ass.h"
#include "../ass-std.h"
IBios* interface;
#endif

#define MAXSTRING 255

#ifdef WIN32
#define _rand() rand()
#else
long rndseed = 0xDEADBEEF;

void srand(long seed)
{
	rndseed = seed;
}

long _rand()
{
	rndseed = (rndseed * 0x41C64E6D) + 0x6073;
	return rndseed;
}
#endif

typedef struct line
{
	struct line* nextLine;
	short lineNo;
	unsigned char lineTokens[MAXSTRING+1];
} line;

typedef unsigned char* sptr;

bool Command();
int Function();

bool errdOut;
sptr ptr;
line* firstLine;
line* currentLine;
int thisLineNum;
char stringVars[27][MAXSTRING+1];
int intVars[27];

bool SyntaxError(const char* message);
int ExpectNumber();
int ExpectExpression();
void SkipWhite();

bool CmdEnd();
bool CmdPrint();
bool CmdInput();
bool CmdLet();
bool CmdRun();
bool CmdNew();
bool CmdList();
bool CmdLoad();
bool CmdSave();
bool CmdCls();
bool CmdColor();
bool CmdLocate();
bool CmdGoto();
bool CmdPoke();

bool FncChr();
bool FncAsc();
bool FncLen();
bool FncAbs();
bool FncRnd();

enum commandTypes
{
	COMMAND, INTFUNC, STRFUNC, EXPR
};

typedef struct command
{
	bool(*handler)();
	const char name[15];
	char type;
} command;

const command newCommands[] = {
	{ 0,{ 0 }, 0 }, //80
	{ CmdEnd, "END", COMMAND }, //81
	{ 0, "FOR", COMMAND }, //82
	{ 0, "NEXT", COMMAND }, //83
	{ CmdPrint, "PRINT", COMMAND }, //84
	{ CmdInput, "INPUT", COMMAND }, //85
	{ CmdLet, "LET", COMMAND }, //86
	{ 0, "REM", COMMAND }, //87
	{ CmdRun, "RUN", COMMAND }, //88
	{ CmdNew, "NEW", COMMAND }, //89
	{ CmdList, "LIST", COMMAND }, //8A
	{ CmdLoad, "LOAD", COMMAND }, //8B
	{ CmdSave, "SAVE", COMMAND }, //8C
	{ CmdCls, "CLS", COMMAND }, //8D
	{ CmdColor, "COLOR", COMMAND }, //8E
	{ CmdLocate, "LOCATE", COMMAND }, //8F

	{ CmdGoto, "GOTO", COMMAND }, //90
	{ CmdPoke, "POKE", COMMAND }, //91
	{ 0,{ 0 }, 0 }, //92
	{ 0,{ 0 }, 0 }, //93
	{ 0,{ 0 }, 0 }, //94
	{ 0,{ 0 }, 0 }, //95
	{ 0,{ 0 }, 0 }, //96
	{ 0,{ 0 }, 0 }, //97
	{ 0,{ 0 }, 0 }, //98
	{ 0,{ 0 }, 0 }, //99
	{ 0,{ 0 }, 0 }, //9A
	{ 0,{ 0 }, 0 }, //9B
	{ 0,{ 0 }, 0 }, //9C
	{ 0,{ 0 }, 0 }, //9D
	{ 0,{ 0 }, 0 }, //9E
	{ 0,{ 0 }, 0 }, //9F

	{ 0,{ 0 }, 0 }, //A0
	{ 0,{ 0 }, 0 }, //A1
	{ 0,{ 0 }, 0 }, //A2
	{ 0,{ 0 }, 0 }, //A3
	{ 0,{ 0 }, 0 }, //A4
	{ 0,{ 0 }, 0 }, //A5
	{ 0,{ 0 }, 0 }, //A6
	{ 0,{ 0 }, 0 }, //A7
	{ 0,{ 0 }, 0 }, //A8
	{ 0,{ 0 }, 0 }, //A9
	{ 0,{ 0 }, 0 }, //AA
	{ 0,{ 0 }, 0 }, //AB
	{ 0,{ 0 }, 0 }, //AC
	{ 0,{ 0 }, 0 }, //AD
	{ 0,{ 0 }, 0 }, //AE
	{ 0,{ 0 }, 0 }, //AF

	{ FncChr, "CHR$", STRFUNC }, //B0
	{ FncAsc, "ASC", INTFUNC }, //B1
	{ FncLen, "LEN", INTFUNC }, //B2
	{ FncAbs, "ABS", INTFUNC }, //B3
	{ 0, "CSRLIN", INTFUNC }, //B4
	{ 0, "POS", INTFUNC }, //B5
	{ FncRnd, "RND", INTFUNC }, //B6
	{ 0,{ 0 }, 0 }, //B7
	{ 0,{ 0 }, 0 }, //B8
	{ 0,{ 0 }, 0 }, //B9
	{ 0,{ 0 }, 0 }, //BA
	{ 0,{ 0 }, 0 }, //BB
	{ 0,{ 0 }, 0 }, //BC
	{ 0,{ 0 }, 0 }, //BD
	{ 0,{ 0 }, 0 }, //BE
	{ 0,{ 0 }, 0 }, //BF

	{ 0,{ 0 }, 0 }, //C0
	{ 0,{ 0 }, 0 }, //C1
	{ 0,{ 0 }, 0 }, //C2
	{ 0,{ 0 }, 0 }, //C3
	{ 0,{ 0 }, 0 }, //C4
	{ 0,{ 0 }, 0 }, //C5
	{ 0,{ 0 }, 0 }, //C6
	{ 0,{ 0 }, 0 }, //C7
	{ 0,{ 0 }, 0 }, //C8
	{ 0,{ 0 }, 0 }, //C9
	{ 0,{ 0 }, 0 }, //CA
	{ 0,{ 0 }, 0 }, //CB
	{ 0,{ 0 }, 0 }, //CC
	{ 0,{ 0 }, 0 }, //CD
	{ 0,{ 0 }, 0 }, //CE
	{ 0,{ 0 }, 0 }, //CF

	{ 0,{ 0 }, 0 }, //D0
	{ 0,{ 0 }, 0 }, //D1
	{ 0,{ 0 }, 0 }, //D2
	{ 0,{ 0 }, 0 }, //D3
	{ 0,{ 0 }, 0 }, //D4
	{ 0,{ 0 }, 0 }, //D5
	{ 0,{ 0 }, 0 }, //D6
	{ 0,{ 0 }, 0 }, //D7
	{ 0,{ 0 }, 0 }, //D8
	{ 0,{ 0 }, 0 }, //D9
	{ 0,{ 0 }, 0 }, //DA
	{ 0,{ 0 }, 0 }, //DB
	{ 0,{ 0 }, 0 }, //DC
	{ 0,{ 0 }, 0 }, //DD
	{ 0,{ 0 }, 0 }, //DE
	{ 0,{ 0 }, 0 }, //DF

	{ 0, ">", EXPR }, //E0
	{ 0, "=", EXPR }, //E1
	{ 0, "<", EXPR }, //E2
	{ 0, "+", EXPR }, //E3
	{ 0, "-", EXPR }, //E4
	{ 0, "*", EXPR }, //E5
	{ 0, "/", EXPR }, //E6
	{ 0, "\\", EXPR }, //E7
	{ 0,{ 0 }, 0 }, //E8
	{ 0,{ 0 }, 0 }, //E9
	{ 0,{ 0 }, 0 }, //EA
	{ 0,{ 0 }, 0 }, //EB
	{ 0,{ 0 }, 0 }, //EC
	{ 0,{ 0 }, 0 }, //ED
	{ 0,{ 0 }, 0 }, //EE
	{ 0,{ 0 }, 0 }, //EF

	{ 0,{ 0 }, 0 }, //F0
	{ 0,{ 0 }, 0 }, //F1
	{ 0,{ 0 }, 0 }, //F2
	{ 0,{ 0 }, 0 }, //F3
	{ 0,{ 0 }, 0 }, //F4
	{ 0,{ 0 }, 0 }, //F5
	{ 0,{ 0 }, 0 }, //F6
	{ 0,{ 0 }, 0 }, //F7
	{ 0,{ 0 }, 0 }, //F8
	{ 0,{ 0 }, 0 }, //F9
	{ 0,{ 0 }, 0 }, //FA
	{ 0,{ 0 }, 0 }, //FB
	{ 0,{ 0 }, 0 }, //FC
	{ 0,{ 0 }, 0 }, //FD
	{ 0,{ 0 }, 0 }, //FE
	{ 0,{ 0 }, 0 }, //FF
};

bool(*commands[])() = {
	0, CmdEnd, /* FOR */ 0, /* NEXT */ 0,
	CmdPrint, CmdInput, CmdLet, /* REM */ 0,
	CmdRun, CmdNew, CmdList, CmdLoad,
	CmdSave, CmdCls, CmdColor, CmdLocate,
	CmdGoto, CmdPoke,
};
bool(*functions[])() = {
	FncChr, FncAsc, FncLen, FncAbs,
	0, 0, FncRnd,
};
const char* cmdNames[] = {
	"", "END", "FOR", "NEXT",
	"PRINT", "INPUT", "LET", "REM",
	"RUN", "NEW", "LIST", "LOAD",
	"SAVE", "CLS", "COLOR", "LOCATE",
	"GOTO", "POKE",
	NULL
};
const char* fncNames[] = {
	"CHR$", "ASC", "LEN", "ABS",
	"CSRLIN", "POS", "RND",
	NULL
};
const char expNames[] = {
	'>', '=', '<', '+', '-', '*', '/', '\\', 0
};

enum byteCodes {
	EOL = 0x00,

	OCTAL = 0x0B,
	HEX,
	//LPRUN = 0x0D,
	//LPSAVE = 0x0E,
	ONEBYTEINT = 0x0F,

	FIRSTSMALLINT = 0x11,
	TWOBYTEINT = 0x1C,

	//FOURBYTEFLOAT = 0x1D,
	//EIGHTBYTEFLOAT = 0x1F,

	FIRSTPRINT = 0x20,

	FIRSTCOMMAND = 0x80,
	PRINT = 0x84,
	GOTO = 0x90,
	FIRSTFUNC = 0xB0,
	FIRSTEXP = 0xE0,
	GT = 0xE0, EQ,
	LT, PLUS, MINUS, MUL,
	DIV, MOD,

};

int attribs = 0x07;

bool SyntaxError(const char* message)
{
	errdOut = true;
	if (thisLineNum != -1)
	{
		printf("\n%s on line %d.\n", message, thisLineNum);
	}
	else
	{
		printf("\n%s.\n", message);
	}
	return false;
}

int ExpectNumber()
{
	SkipWhite();
	bool neg = false;
	int ret = 0;
	if (*ptr == MINUS)
	{
		neg = true;
		ptr++;
	}
	if (*ptr >= FIRSTSMALLINT && *ptr <= FIRSTSMALLINT + 10)
	{
		ret = (*ptr++) - FIRSTSMALLINT;
	}
	else if (*ptr == ONEBYTEINT)
	{
		ptr++;
		ret = *ptr++;
	}
	else if (*ptr == TWOBYTEINT)
	{
		ptr++;
		ret = *ptr++;
		ret |= (*ptr++) << 8;
	}
	else if (*ptr >= FIRSTFUNC && *ptr < FIRSTEXP)
	{
		int f = Function();
		if (f == 1)
			return intVars[26];
		else if (f == 2)
		{
			SyntaxError("Expected int-returning function");
			return 0;
		}
		ptr--;
	}
	if (neg)
		ret = -ret;
	return ret;
}

int ExpectExpression()
{
	int result = 0;
	int term = ExpectNumber();
	sptr oldPtr = ptr;
	SkipWhite();
	if (*ptr >= FIRSTEXP)
	{
		if (*ptr == LT)
		{
			ptr++;
			if (*ptr == GT)
			{
				ptr++;
				result = term != ExpectExpression();
			}
			else
			{
				result = term < ExpectExpression();
			}
		}
		else if (*ptr == EQ)
		{
			ptr++;
			result = term == ExpectExpression();
		}
		else if (*ptr == GT)
		{
			ptr++;
			result = term > ExpectExpression();
		}

		else if (*ptr == MUL)
		{
			ptr++;
			result = term * ExpectExpression();
		}
		else if (*ptr == DIV)
		{
			ptr++;
			int term2 = ExpectExpression();
			if (term2 == 0)
			{
				SyntaxError("Division by zero");
				return 0;
			}
			result = term / term2;
		}
		else if (*ptr == MOD)
		{
			ptr++;
			int term2 = ExpectExpression();
			if (term2 == 0)
			{
				SyntaxError("Modulo by zero");
				return 0;
			}
			result = term % term2;
		}
		else if (*ptr == PLUS)
		{
			ptr++;
			result = term + ExpectExpression();
		}
		else if (*ptr == MINUS)
		{
			ptr++;
			result = term - ExpectExpression();
		}
	}
	else
	{
		ptr = oldPtr;
		result = term;
	}
	return result;
}

char* ExpectString()
{
	sptr oldPtr = ptr;
	SkipWhite();
	if (*ptr != '"')
		return NULL;
	ptr++;
	int len = 0;
	while (*ptr++ != '"')
		len++;
	ptr = oldPtr + 1;
	char* ret = (char*)malloc(len + 1);
	char* r = ret;
	while (len--)
	{
		*r++ = *ptr++;
	}
	*r++ = 0;
	ptr++;
	return ret;
}

void SkipWhite()
{
	while (*ptr == ' ')
		ptr++;
}

bool Anticipate(unsigned char what)
{
	sptr oldPtr = ptr;
	SkipWhite();
	if (*ptr == what)
	{
		ptr++;
		return true;
	}
	ptr = oldPtr;
	return false;
}

bool CmdEnd()
{
	return false;
}

bool CmdPrint()
{
	bool hadSemiColon = false;
	SkipWhite();
	while (*ptr && !errdOut)
	{
		unsigned char c = *ptr;
		if (c == EOL || c == ':')
			break;
		if (c == '"')
		{
			char* str = ExpectString();
			printf(str);
			free(str);
			ptr--;
		}
		else if (isalpha(c))
		{
			if (*(ptr + 1) == '$')
			{
				ptr++;
				printf(stringVars[c - 'A']);
			}
			else
			{
				printf("%d", intVars[c - 'A']);
			}
		}
		else if (c == ';')
		{
			hadSemiColon = true;
		}
		else if (c == ',')
		{
			printf("\t");
			hadSemiColon = true; //...ish
		}
		else if (c >= FIRSTFUNC && c < FIRSTEXP)
		{
			int f = Function();
			if (f == 1)
				printf("%d", intVars[26]);
			else if (f == 2)
				printf("%s", stringVars[26]);
			ptr--;
			hadSemiColon = false;
		}
		else
		{
			printf("%i", ExpectExpression());
			ptr--;
		}
		ptr++;
	}
	if (!hadSemiColon)
		printf("\n");
	return true;
}

bool CmdInput()
{
	bool hadSemiColon = false;
	signed int dest = -1;
	bool string = false;
	while (*ptr++)
	{
		unsigned char c = *ptr;
		if (c == EOL || c == ':')
			break;
		if (c >= 'A' && c <= 'Z')
		{
			if (dest == -1)
				dest = c - 'A';
			else
				return SyntaxError("Expected a variable");
			continue;
		}
		else if (c == '$')
		{
			if (dest != -1)
				string = true;
			else if (c != '%')
				return SyntaxError("Expected a variable");
			continue;
		}
		if (dest == -1)
		{
			if (c == '"')
			{
				char* str = ExpectString();
				printf(str);
				free(str);
				ptr--;
			}
			else if (c == ';')
			{
				hadSemiColon = true;
			}
			else if (c == ',')
			{
				printf("\t");
				hadSemiColon = true; //...ish
			}
		}
		else
		{
			//return SyntaxError();
		}
	}
	if (!hadSemiColon)
		printf("? ");
	if (string)
	{
		gets_s(stringVars[dest], MAXSTRING);
	}
	else
	{
		char t[16];
		gets_s(t, 16);
		intVars[dest] = atoi(t);
	}
	return true;
}

bool CmdLet()
{
	signed int dest = -1;
	bool string = false;
	SkipWhite();
	unsigned char c = *ptr;
	if (isalpha(c))
	{
		dest = c - 'A';
		ptr++;
		if (*ptr == '$')
		{
			ptr++;
			string = true;
		}
		else if (*ptr == '%')
		{
			ptr++;
			string = false;
		}
	}
	else
		return SyntaxError("Expected a variable");
	if (!Anticipate(EQ))
		return SyntaxError("Expected '='");
	if (string)
	{
		char* t = ExpectString();
		strcpy(stringVars[dest], t + 1);
		free(t);
	}
	else
	{
		intVars[dest] = ExpectExpression();
	}
	return true;
}

bool FncChr()
{
	if (!Anticipate('(')) return SyntaxError("Expected '('");
	sprintf(stringVars[26], "%c", ExpectExpression());
	if (errdOut) return false;
	if (!Anticipate(')')) return SyntaxError("Expected ')'");
	return true;
}

bool FncAsc()
{
	if (!Anticipate('(')) return SyntaxError("Expected '('");
	char* t = ExpectString();
	if (strlen(t) == 0) return SyntaxError("Invalid string");
	intVars[26] = t[0];
	free(t);
	if (!Anticipate(')')) return SyntaxError("Expected ')'");
	return true;
}

bool FncLen()
{
	if (!Anticipate('(')) return SyntaxError("Expected '('");
	char* t = ExpectString();
	intVars[26] = strlen(t);
	free(t);
	if (!Anticipate(')')) return SyntaxError("Expected ')'");
	return true;
}

bool FncAbs()
{
	if (!Anticipate('(')) return SyntaxError("Expected '('");
	int i = ExpectExpression();
	if (i < 0) i = -i;
	intVars[26] = i;
	if (!Anticipate(')')) return SyntaxError("Expected ')'");
	return true;
}

bool FncRnd()
{
	if (!Anticipate('(')) return SyntaxError("Expected '('");
	int range = ExpectExpression();
	intVars[26] = _rand() % range;
	if (intVars[26] < 0) intVars[26] = -intVars[26];
	if (!Anticipate(')')) return SyntaxError("Expected ')'");
	return true;
}

bool Command()
{
	unsigned char c = *ptr;
	if (c == '?') c = PRINT;
	ptr++;
	bool(*func)() = commands[c - FIRSTCOMMAND];
	if (func)
		return func();
	return false;
}

int Function()
{
	unsigned char c = *ptr++;
	bool(*func)() = functions[c - FIRSTFUNC];
	if (func && fncNames[c - FIRSTFUNC][strlen((char*)fncNames[c - FIRSTFUNC]) - 1] != '$')
		return func() ? 1 : 0;
	else
		return func() ? 2 : 0;
	return 0;
}

bool CmdList()
{
	int from = 0, to = 0;
	SkipWhite();
	if (*ptr != EOL && *ptr != '-')
		from = ExpectNumber();
	SkipWhite();
	if (*ptr == '-')
	{
		ptr++;
		to = ExpectNumber();
	}

	if (firstLine == 0)
	{
		printf("No program!\n");
		return false;
	}
	line* thisLine = firstLine;
	while (true)
	{
		if (thisLine->lineNo < from)
		{
			thisLine = thisLine->nextLine;
			if (to > from && thisLine->lineNo > to)
				break;
			continue;
		}
		printf("%-4d ", thisLine->lineNo);
		sptr ptr = thisLine->lineTokens;
		while (true)
		{
			unsigned char c = *ptr;
			if (c >= FIRSTCOMMAND && c < FIRSTFUNC)
				printf(cmdNames[c - FIRSTCOMMAND]);
			if (c >= FIRSTFUNC && c < FIRSTEXP)
				printf(fncNames[c - FIRSTFUNC]);
			else if (c >= 0x20 && c < FIRSTCOMMAND)
				printf("%c", c);
			else if (c >= FIRSTSMALLINT && c <= FIRSTSMALLINT + 10)
				printf("%d", c - FIRSTSMALLINT);
			else if (c == ONEBYTEINT)
			{
				printf("%i", *(char*)(ptr + 1));
				ptr += 1;
			}
			else if (c == TWOBYTEINT)
			{
				printf("%i", *(short*)(ptr + 1));
				ptr += 2;
			}
			else if (c == EOL)
			{
				//if (nextLineOffset == 0)
				if (thisLine->nextLine == 0)
				{
					printf("\n");
					return true;
				}
				else
				{
					printf("\n");
					thisLine = thisLine->nextLine;
					break;
				}
			}
			ptr++;
		}
	}
	return true;
}

bool CmdLoad()
{
	SkipWhite();
	if (*ptr != '"') return SyntaxError("Expected string");
	ptr++;
	char file[MAXSTRING];
	char* fn = file;
	while (*ptr != '"')
	{
		*fn++ = *ptr++;
	}
	*fn = 0;

	FILE* f = fopen(file, "rb");
	if ((int)f > 20)
	{
		CmdNew();
		line* previousLine = 0;
		short nextLineAt = 0;
		while (true)
		{
			line* thisLine = (line*)malloc(sizeof(line));
			memset((void*)thisLine, 0, sizeof(line));
			fread(&nextLineAt, sizeof(short), 1, f);
			fread(&thisLine->lineNo, sizeof(short), 1, f);
			fgets((char*)thisLine->lineTokens, MAXSTRING, f);
			if (previousLine != 0)
				previousLine->nextLine = thisLine;
			if (firstLine == 0)
				firstLine = thisLine;
			if (thisLine->lineNo == 0)
			{
				//BUGBUG
				printf("<line zero bug, skipping>\n");
				free(thisLine);
				thisLine = 0;
			}
			if (nextLineAt == 0)
				break;
			previousLine = thisLine;
			fseek(f, nextLineAt, SEEK_SET);
		}
	}
	else
	{
		printf("Could not open %s: %s\n", file, DISK->FileErrStr((int)f));
		return false;
	}
	fclose(f);
	return true;
}

bool CmdSave()
{
	SkipWhite();
	if (*ptr != '"') return SyntaxError("Expected string");
	ptr++;
	char file[MAXSTRING];
	char* fn = file;
	while (*ptr != '"')
	{
		*fn++ = *ptr++;
	}
	*fn = 0;

	FILE* f = fopen(file, "wb");
	if ((int)f > 20)
	{
		line* thisLine = firstLine;
		unsigned short offset = 0;
		while (thisLine)
		{
			line* nextLine = thisLine->nextLine;
			if (thisLine->lineNo == 0)
			{
				//BUGBUG
				printf("<line zero bug, skipping>\n");
				thisLine = nextLine;
				continue;
			}
			unsigned short lineLen = 1;
			for (int i = 0; i < MAXSTRING; i++, lineLen++)
			{
				if (thisLine->lineTokens[i] == EOL)
					break;
			}
			unsigned short nextOffset = offset + lineLen + 2 + 2;
			if (nextLine == NULL) nextOffset = 0;
			fwrite(&nextOffset, sizeof(short), 1, f);
			fwrite(&thisLine->lineNo, sizeof(short), 1, f);
			fwrite(&thisLine->lineTokens, sizeof(char), lineLen, f);
			thisLine = nextLine;
			offset = nextOffset;
		}
	}
	else
	{
		printf("Could not open %s: %s\n", file, DISK->FileErrStr((int)f));
		return false;
	}
	fclose(f);
	return true;
}

bool CmdCls()
{
#ifdef WIN32
	COORD tl = { 0,0 };
	CONSOLE_SCREEN_BUFFER_INFO s;
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(console, &s);
	DWORD written, cells = s.dwSize.X * s.dwSize.Y;
	FillConsoleOutputCharacter(console, ' ', cells, tl, &written);
	FillConsoleOutputAttribute(console, attribs, cells, tl, &written);
	SetConsoleCursorPosition(console, tl);
#else
	TEXT->ClearScreen();
#endif
	return true;
}

bool CmdColor()
{
	int fg = attribs & 0xF;
	int bg = (attribs >> 4) & 0xF;

	if (!Anticipate(','))
	{
		fg = ExpectExpression();
		if (fg < 0 || fg > 15) return SyntaxError("Value out of range");
	}
	else
		ptr--;
	if (Anticipate(','))
	{
		bg = ExpectExpression();
		if (bg < 0 || bg > 15) return SyntaxError("Value out of range");
	}
	attribs = fg | (bg << 4);

#ifdef WIN32
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(console, attribs);
#else
	TEXT->SetTextColor(bg, fg);
#endif
	return true;
}

bool CmdLocate()
{
#ifdef WIN32
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(console, &info);
	short row = info.dwCursorPosition.X + 1;
	short col = info.dwCursorPosition.Y + 1;
#else
	short row = 1;
	short col = 1;
#endif
	if (!Anticipate(','))
	{
		row = ExpectExpression();
		if (row < 1 || row > 25) return SyntaxError("Value out of range");
	}
	else
		ptr--;
	if (Anticipate(','))
	{
		col = ExpectExpression();
		if (col < 1 || col > 80) return SyntaxError("Value out of range");
	}
#ifdef WIN32
	COORD pos = { col - 1, row - 1 };
	SetConsoleCursorPosition(console, pos);
#else
	TEXT->SetCursorPosition(col - 1, row - 1);
#endif
	return true;
}

bool CmdGoto()
{
	if (thisLineNum == -1)
		return SyntaxError("GOTO not available in immediate mode");
	int target = ExpectExpression();
	if (errdOut) return false;
	//printf("[GOTO: target = %d]", target);
	line* thisLine = firstLine;
	while (thisLine->lineNo != target)
	{
		ptr = thisLine->lineTokens;
		if (thisLine->lineNo > target)
		{
			SyntaxError("Could not find target line");
			return false;
		}
	}
	currentLine = thisLine;
	ptr = currentLine->lineTokens;
	return true;
}

bool CmdPoke()
{
#ifdef WIN32
	int ptr = ExpectExpression();
	if (Anticipate(','))
	{
		int val = ExpectExpression();
		return true;
	}
	else
		return SyntaxError("Syntax error");
#else
	int ptr = ExpectExpression() + MEM_IO;
	if (Anticipate(','))
	{
		int val = ExpectExpression();
		*(char*)ptr = val;
		return true;
	}
	else
		return SyntaxError("Syntax error");
#endif
}

int Compile(const char* input, sptr output)
{
	unsigned char codeSoFar[MAXSTRING * 2] = "";
	char token[64] = "";
	sptr ptr = codeSoFar;
	char* tok = token;
	int mode = 0;
	int num = 0;
	while (*input > 0 && *input != (char)0xFE)
	{
		unsigned char c = *input;
		if (mode == 0) //OUT mode
		{
			if (isdigit(c))
			{
				mode = 2; //NUM mode
				num = c - '0';
				input++;
				continue;
			}
			if (isalpha(c))
			{
				c = toupper(c);
				unsigned char n = *(input + 1);
				n = toupper(n);
				if (strlen(token) < 2 && (n == '$' || n == '%' || !isalpha(n)))
				{
					*ptr++ = c;
				}
				else
				{
					*tok++ = c;
					if (n == '$')
					{
						*tok++ = n;
						input++;
					}
					int cmdFound = -1;
					for (int i = 0; i < FIRSTFUNC - FIRSTCOMMAND; i++)
					{
						if (cmdNames[i] == NULL) break;
						if (strcmp((char*)cmdNames[i], token) == 0)
							cmdFound = i;
					}
					if (cmdFound >= 0)
					{
						memset(token, 0, sizeof(token));
						tok = token;
						*ptr++ = FIRSTCOMMAND + cmdFound;
					}
					else
					{
						for (int i = 0; i < 16; i++)
						{
							if (fncNames[i] == NULL) break;
							if (strcmp((char*)fncNames[i], token) == 0)
								cmdFound = i;
						}
						if (cmdFound >= 0)
						{
							memset(token, 0, sizeof(token));
							tok = token;
							*ptr++ = FIRSTFUNC + cmdFound;
						}

					}
				}
			}
			else
			{
				int expFound = -1;
				for (int i = 0; i < 16; i++)
				{
					if (expNames[i] == 0) break;
					if (expNames[i] == c)
					{
						expFound = i;
						break;
					}
				}
				if (expFound >= 0)
				{
					*ptr++ = FIRSTEXP + expFound;
					input++;
					continue;
				}
				*ptr++ = c;
				if (c == '\"')
					mode = 1; //to QUOTE mode;
			}
		}
		else if (mode == 1) //QUOTE mode
		{
			*ptr++ = c;
			if (c == '\"')
				mode = 0; //to OUT mode
		}
		else if (mode == 2) //NUM mode
		{
			if (isdigit(c))
			{
				num = (num * 10) + (c - '0');
			}
			else
			{
emitNum:
				mode = 0; //to OUT mode
				if (num <= 10)
				{
					*ptr++ = FIRSTSMALLINT + num;
				}
				else if (num <= 255)
				{
					*ptr++ = ONEBYTEINT;
					*ptr++ = num;
				}
				else
				{
					*ptr++ = TWOBYTEINT;
					*ptr++ = num & 0xFF;
					*ptr++ = (num >> 8) & 0xFF;
				}
				continue;
			}
		}
		input++;
	}
	if (mode == 2)
		goto emitNum;
	*ptr++ = 0;
	memcpy(output, codeSoFar, ptr - codeSoFar);
	return ptr - codeSoFar;
}

int CompileLine(const char* directInput)
{
	//It's a numbered command.
	char* input = strdup(directInput);
	char* toCompile = input;
	for (int i = 0; i < 5; i++)
	{
		if (input[i] == ' ')
		{
			input[i] = 0;
			toCompile += i + 1;
			break;
		}
		if (input[i] == '\0')
		{
			toCompile = (char*)1;
			break;
		}
	}
	if (toCompile == input)
	{
		printf("Invalid numbered input.\n");
		return 0;;
	}
	int lineNo = atoi(input);
	if (toCompile == (char*)1)
	{
		//Delete a line!
		if (firstLine == 0)
			return 0;
		line *thisLine = firstLine;
		line *prevLine = 0;
		while (true)
		{
			if (thisLine->lineNo >= lineNo)
			{
				if (prevLine == 0)
					firstLine = thisLine->nextLine;
				else
					prevLine->nextLine = thisLine->nextLine;
				free((void*)thisLine);
				break;
			}
			prevLine = thisLine;
			thisLine = thisLine->nextLine;
		}
		return 1;
	}
	if (firstLine == 0)
	{
		firstLine = (line*)malloc(sizeof(line));
		memset((void*)firstLine, 0, sizeof(line));
		firstLine->lineNo = lineNo;
		Compile(toCompile, firstLine->lineTokens);
	}
	else
	{
		line *prevLine = 0;
		line *nextLine = 0;
		line *thisLine = firstLine;
		while (true)
		{
			if (thisLine->lineNo >= lineNo)
			{
				nextLine = thisLine;
				break;
			}
			prevLine = thisLine;
			if (thisLine->nextLine == 0)
				break;
			thisLine = thisLine->nextLine;
		}
		if (thisLine->lineNo != lineNo)
		{
			thisLine = (line*)malloc(sizeof(line));
			memset((void*)thisLine, 0, sizeof(line));
		}
		if (prevLine == 0)
			firstLine = thisLine;
		if (prevLine != 0)
			prevLine->nextLine = thisLine;
		if (nextLine != 0 && nextLine != thisLine)
			thisLine->nextLine = nextLine;
		thisLine->lineNo = lineNo;
		Compile(toCompile, thisLine->lineTokens);
	}
	return 1;
}

int RunLine()
{
	errdOut = false;
	while (!errdOut)
	{
		unsigned char c = *ptr;
		if (c == '?') c = PRINT;
		if (c >= FIRSTCOMMAND && c <= FIRSTFUNC)
		{
			if (!Command())
			{
				return 1;
			}
		}
		if (c == GOTO) return 2;
		c = *ptr;
		if (c == ':')
		{
			ptr++;
			continue;
		}
		else if (c == EOL)
		{
			return 0;
		}
		ptr++;
	}
	return -1;
}

int Direct(const char* directInput)
{
	unsigned char directCode[MAXSTRING + 1];
	Compile(directInput, directCode);
	ptr = directCode;
	thisLineNum = -1;
	return RunLine();
}

bool CmdRun()
{
	if (firstLine == 0)
	{
		printf("No program!\n");
		return false;
	}
	currentLine = firstLine;
	while (true)
	{
		ptr = currentLine->lineTokens;
		thisLineNum = currentLine->lineNo;
		int ret = RunLine();
		if (ret == 0) //End of line
		{
			if (currentLine->nextLine == 0)
				break;
			currentLine = currentLine->nextLine;
		}
		else if (ret == 1) //Halted
			break;
		else if (ret == 2) //Goto'd
			continue;
	}
	return true;
}

bool CmdNew()
{
	line* thisLine = firstLine;
	while (thisLine)
	{
		line* nextLine = thisLine->nextLine;
		free(thisLine);
		thisLine = nextLine;
	}
	firstLine = NULL;
	return true;
}

bool List(int from, int to)
{
	char t[64];
	sprintf(t, "LIST %d-%d", from, to);
	Direct(t);
	return true;
}

int Load(const char* file)
{
	char t[MAXSTRING + 16];
	sprintf(t, "LOAD \"%s\"", file);
	return Direct(t);
}

int Save(const char* file)
{
	char t[MAXSTRING + 16];
	sprintf(t, "SAVE \"%s\"", file);
	return Direct(t);

}

#ifdef WIN32
int main(int argc, char* argv[])
{
	Direct("COLOR 14: PRINT \"A3X BASIC (win32 v.)\": COLOR 7");
#else
int main()
{
	intoff();
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	REG_CARET = 0x8000;
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	Direct("COLOR 14: PRINT \"A3X BASIC\": COLOR 7");
#endif


	//Direct("print \"Hello, world!\"");
	//CompileLine("10 let b = 1337");
	//CompileLine("20 print b");
	//Direct("print 42 * 10");
	//List(20, 0);
	//CmdRun();
	//Direct("input b");
	//Direct("print b%");
	//Direct("color 14, 1: cls: locate 2, 4: locate 7: print \"lol?\": color 12: print \"hi\": color ,4: print \"lo\"");
	//return 0;

	//Load("program.bas");
	//Compile("PRINT \"rage cage\"", code + 4);
	//printf(">LIST\n");
	//Direct("LIST");
	//Save("pragrom.bas");
	//printf(">RUN\n");
	//Run();

	char directInput[MAXSTRING+1];
	while(true)
	{
		printf(">");
		gets_s(directInput, MAXSTRING);
		if (isdigit(directInput[0]))
		{
			CompileLine(directInput);
		}
		else
		{
			Direct(directInput);
		}
	}

	getchar();
	return 0;
}
