#include "..\ass.h"

typedef struct
{
	uint16_t sciShit;
	uint16_t lowChar;
	uint16_t highChar;
	uint16_t pointSize;
	uint16_t charRecs[];
} TFont;

extern const TFont* const fonts[];

#define BYTESWAP(x) x = ((x & 0xFF00) >> 8) | ((x & 0xFF) << 8);

uint8_t canvas[0x3C00] = { 0 };

void plotTilePixel(int x, int y, unsigned char clr)
{
	uint8_t *dest = (uint8_t*)(canvas + ((x / 8) * 32) + ((y / 8) * 1024)) + ((y % 8) * 4) + ((x % 8) / 2);
	uint8_t shift = (x % 2) << 2;
	*dest = (*dest & (0xF0 >> shift)) | ((clr & 15) << shift);
}

int drawChar(unsigned int ch, int x, int y, int col, int font)
{
	ch = ch & 0xFF;
	const TFont* f = fonts[font];
	uint16_t high = f->highChar;
	BYTESWAP(high);
	if (ch >= high) ch = '?';
	uint16_t offset = f->charRecs[ch];
	BYTESWAP(offset);
	uint8_t* charAddr = (uint8_t*)((uint8_t*)f->charRecs + offset - 6);
	uint8_t width = *charAddr++;
	uint8_t height = *charAddr++;
	uint8_t b = 0;
	for (int i = 0; i < height; i++)
	{
		for (int done = 0; done < width; done++)
		{
			if ((done & 7) == 0)
				b = *charAddr++;
			if (b & 0x80)
				plotTilePixel(done + x, i + y, col);
			b = b << 1;
		}
	}
	return width;
}

int drawString(const char* str, int x, int y, int col, int font)
{
	while(*str)
	{
		x += drawChar(*str++, x, y, col, font);
	}
	MISC->DmaCopy(TILESET + 0xC400, (int8_t*)&canvas, 0x3C00, DMA_BYTE);
	return x;
}

int measureString(const char* str, int font)
{
	const TFont* f = fonts[font];
	uint16_t high = f->highChar;
	BYTESWAP(high);
	int ret = 0;
	while(*str)
	{
		unsigned int ch = (unsigned int)*str++;
		ch = ch & 0xFF;
		if (ch >= high) ch = '?';
		uint16_t offset = f->charRecs[ch];
		BYTESWAP(offset);
		uint8_t* charAddr = (uint8_t*)((uint8_t*)f->charRecs + offset - 6);
		ret += *charAddr++;
	}
	return ret;
}
