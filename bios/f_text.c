#include "../ass.h"
#include "funcs.h"

extern int32_t vsprintf(char*, const char*, va_list);

const ITextLibrary textLibrary =
{
	Write, Format, VFormat, WriteChar,
	SetCursorPosition, SetTextColor, ClearScreen,
};

#define TAB_STOPS 8

//Kill our printf define for __attribute's sake
#ifdef printf
#undef printf
#endif

void ScrollIfNeeded()
{
	int cursorPos = REG_CARET & 0x3FFF;
	int textWidth = (REG_SCREENMODE & SMODE_320) ? 40 : 80;
	int textHeight = (REG_SCREENMODE & SMODE_240) ? 30 : 60;
	if (cursorPos / textWidth >= textHeight - 1)
	{
		int32_t *dst = (int32_t*)MEM_VRAM;
		int32_t *src = dst + (textWidth / 2);
		for (int32_t i = 0; i < (textWidth * (textHeight - 1)) / 2; i++)
			*dst++ = *src++;
		int16_t* bottomLine = ((int16_t*)MEM_VRAM) + (textWidth * (textHeight - 1));
		for (int32_t i = 0; i < textWidth; i++)
			*bottomLine++ = (' ' << 8) | interface->io.attribs;
		cursorPos = textWidth * (textHeight - 2);
		REG_CARET = (REG_CARET & 0xC000) | cursorPos;
	}
}

__attribute((format (printf, 1, 2)))
int Write(const char* format, ...)
{
	char buffer[1024];
	int caret = REG_CARET & 0x8000;
	REG_CARET &= ~0x8000;
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	char *b = buffer;
	int len = 0;
	while (*b)
	{
		//This is incredibly silly.
		/*
		if (*b == 0x1B)
		{
			if (*(b+1) == 'E')
			{
				interface->io.attribs |= 8;
				b += 2;
				len += 2;
				continue;
			}
			else if (*(b+1) == 'e')
			{
				interface->io.attribs &= ~8;
				b += 2;
				len += 2;
				continue;
			}
		}
		*/
		WriteChar(*b++);
		len++;
	}
	va_end(args);
	REG_CARET |= caret;
	return len;
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

int VFormat(char* buffer, const char* format, va_list args)
{
	//TEST ME
	return vsprintf(buffer, format, args);
}

void WriteChar(char ch)
{
	int cursorPos = REG_CARET & 0x3FFF;
	int textWidth = (REG_SCREENMODE & SMODE_320) ? 40 : 80;
	if (ch == '\r')
		;
	else if (ch == '\n')
		cursorPos = (cursorPos / textWidth * textWidth) + textWidth;
	else if (ch == '\t')
		cursorPos += TAB_STOPS - (cursorPos % TAB_STOPS);
	else if (ch == '\b')
	{
		((int16_t*)MEM_VRAM)[--cursorPos] = (' ' << 8) | interface->io.attribs;
	}
	else
		((int16_t*)MEM_VRAM)[cursorPos++] = (ch << 8) | interface->io.attribs;
	REG_CARET = (REG_CARET & 0xC000) | cursorPos;
	ScrollIfNeeded();
}

//...and redef it
#define printf Write

void SetCursorPosition(int left, int top)
{
	int textWidth = (REG_SCREENMODE & SMODE_320) ? 40 : 80;
	REG_CARET = (REG_CARET & 0xC000) | ((textWidth * top) + left);
}

void SetTextColor(int back, int fore)
{
	interface->io.attribs = (back << 4) | fore;
}

void ClearScreen()
{
	if ((REG_SCREENMODE & 3) == 0)
	{
		DmaClear((void*)MEM_VRAM, ((' ' << 8) | interface->io.attribs), 80*60*2, DMA_SHORT);
		REG_CARET = (REG_CARET & 0xC000);
	}
	else
	{
		DmaClear((void*)MEM_VRAM, 0, 640 * 480, DMA_BYTE);
	}
}
