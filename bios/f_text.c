#include "../ass.h"
#include "funcs.h"

extern int32_t vsprintf(char*, const char*, va_list);

void(*oldVBlank)(void);
void(*oldHBlank)(void);

const ITextLibrary textLibrary =
{
	Write, Format, WriteChar,
	SetBold,
	//WriteString, WriteInt, WriteCharacter,
	//WriteHex, WriteHex8, WriteHex16, WriteHex32,
	SetCursor, SetTextColor, ClearScreen,
};

#define TEXT_WIDTH 80
#define TEXT_HEIGHT 25
#define TAB_STOPS 8
int32_t cursorPos;
unsigned char attribs, textWidth, textHeight;

//Kill our printf define for __attribute's sake
#ifdef printf
#undef printf
#endif

#ifdef DEBUG
__attribute((format (printf, 1, 2)))
void dpf(const char* format, ...)
{
	char buffer[1024] = "[DEBUG] ";
	intoff();
	va_list args;
	va_start(args, format);
	vsprintf(buffer + 8, format, args);
	char* b = buffer;
	while(*b)
		*(unsigned char*)(MEM_IO+0x000E) = *b++;
	*(unsigned char*)(MEM_IO+0x000E) = '\n';
	va_end(args);
	inton();
}
#endif

void ScrollIfNeeded()
{
	if (cursorPos / textWidth >= textHeight - 1)
	{
		int32_t *dst = (int32_t*)MEM_VRAM;
		int32_t *src = dst + (textWidth / 2);
		for (int32_t i = 0; i < (textWidth * (textHeight - 1)) / 2; i++)
			*dst++ = *src++;
		int16_t* bottomLine = ((int16_t*)MEM_VRAM) + (textWidth * (textHeight - 1));
		for (int32_t i = 0; i < textWidth; i++)
			*bottomLine++ = (' ' << 8) | attribs;
		cursorPos = textWidth * (textHeight - 2);
	}
}

__attribute((format (printf, 1, 2)))
int Write(const char* format, ...)
{
	char buffer[1024];
	intoff();
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	char *b = buffer;
	while (*b)
		WriteChar(*b++);
	va_end(args);
	inton();
	return buffer - b;
}

__attribute((format (printf, 2, 3)))
int Format(char* buffer, const char* format, ...)
{
	int ret;
	va_list args;
	va_start(args, format);
	ret = vsprintf(buffer, format, args);
	va_end(args);
	return ret;
}

void WriteChar(char ch)
{
	if (ch == '\r')
		;
	else if (ch == '\n')
		cursorPos = (cursorPos / textWidth * textWidth) + textWidth;
	else if (ch == '\t')
		cursorPos = (cursorPos / TAB_STOPS * TAB_STOPS) + TAB_STOPS;
	else if (ch == '\b')
	{
		((int16_t*)MEM_VRAM)[--cursorPos] = (' ' << 8) | attribs;
	}
	else
		((int16_t*)MEM_VRAM)[cursorPos++] = (ch << 8) | attribs;
	ScrollIfNeeded();
}

//...and redef it
#define printf Write

void SetBold(int32_t bold)
{
	if (bold)
		REG_SCREENMODE |= SMODE_BOLD;
	else
		REG_SCREENMODE &= ~SMODE_BOLD;
}

void SetCursor(int32_t left, int32_t top)
{
	cursorPos = (textWidth * top) + left;
}

void SetTextColor(int32_t back, int32_t fore)
{
	attribs = (back << 4) | fore;
}

void ClearScreen()
{
	if ((REG_SCREENMODE & 3) == 0)
	{
		DmaClear((void*)MEM_VRAM, ((' ' << 8) | attribs), 80*60*2, DMA_SHORT);
		cursorPos = 0;
	}
	else
	{
		DmaClear((void*)MEM_VRAM, 0, 640 * 480, DMA_BYTE);
	}
}

