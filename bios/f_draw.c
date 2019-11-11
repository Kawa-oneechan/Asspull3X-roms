#include "../ass.h"
#include "funcs.h"

extern int32_t vsprintf(char*, const char*, va_list);

const IDrawingLibrary drawingLibrary =
{
	ResetPalette, DisplayPicture,
	FadeToBlack, FadeFromBlack,
	FadeToWhite, FadeFromWhite,
	DrawString, DrawFormat, DrawChar
};

/* CONSIDER: We have four different copies of this routine after preprocessing.
 * The only difference between them is that two of them work on 4bpp data.
 * One idea might be to have two actual DrawChar functions, one for each bitdepth,
 * and a quick check on what the current screen mode is to decide on the width.
 */

#define DRAWCHAR4(WIDTH) \
	char* glyph = interface->DrawCharFont + (c * (interface->DrawCharHeight & 0x00FF)); \
	char* target = (char*)MEM_VRAM + (y * (WIDTH/2)) + (x / 2); \
	if (x % 2 == 0) { for (int32_t line = 0; line < (interface->DrawCharHeight & 0x00FF); line++) { \
			char g = *glyph++; for (int32_t bit = 0; bit < 8; bit += 2) { \
				int32_t p = g >> bit; \
				if (p & 1) *target = (*target & 0x0F) | (color << 4); \
				if (p & 2) *target = (*target & 0xF0) | color; \
				target++; } target += (WIDTH/2) - 4; \
		} }	else { for (int32_t line = 0; line < (interface->DrawCharHeight & 0x00FF); line++) { \
			char g = *glyph++; if (g & 1) *target = (*target & 0xF0) | color; \
			for (int32_t bit = 1; bit < 7; bit += 2) { target++; int32_t p = g >> bit; \
				if (p & 1) *target = (*target & 0x0F) | (color << 4); \
				if (p & 2) *target = (*target & 0xF0) | color; \
			} target++; if ((g >> 7) & 1) *target = (*target & 0x0F) | (color << 4); \
			target += (WIDTH/2) - 4; } }
#define DRAWCHAR8(WIDTH) \
	char* glyph = interface->DrawCharFont + (c * (interface->DrawCharHeight & 0x00FF)); \
	char* target = (char*)MEM_VRAM + (y * (WIDTH)) + x; \
	for (int32_t line = 0; line < (interface->DrawCharHeight & 0x00FF); line++) { for (int32_t bit = 0; bit < 8; bit++) { \
			int32_t pixel = (*glyph >> bit) & 1; if (pixel == 0) continue; \
			target[bit] = color; \
		}  glyph++; target += (WIDTH); }

void DrawChar4_320(char c, int32_t x, int32_t y, int32_t color) { DRAWCHAR4(320) }
void DrawChar4_640(char c, int32_t x, int32_t y, int32_t color) { DRAWCHAR4(640) }
void DrawChar8_320(char c, int32_t x, int32_t y, int32_t color) { DRAWCHAR8(320) }
void DrawChar8_640(char c, int32_t x, int32_t y, int32_t color) { DRAWCHAR8(640) }

static const uint16_t palette[] = {
	0x0000, 0x5400, 0x02A0, 0x56a0, 0x0015, 0x5415, 0x0115, 0x56B5,
	0x294A, 0x7D4A, 0x2BEA, 0x7FEA, 0x295F, 0x7D5F, 0x2BFF, 0x7FFF,
};


void ResetPalette()
{
	for (int32_t idx = 0; idx < 16; idx++)
	{
		PALETTE[idx] = palette[idx];
	}
}

void DisplayPicture(TImageFile* picData)
{
	int32_t mode = -1;
	if (picData->Width == 320)
	{
		if (picData->Height == 240 || picData->Height == 200)
			mode = SMODE_320 | SMODE_240;
		else if (picData->Height == 480 || picData->Height == 400)
			mode = SMODE_320;
	}
	else if (picData->Width == 640)
	{
		if (picData->Height == 240 || picData->Height == 200)
			mode = SMODE_240;
		else if (picData->Height == 480 || picData->Height == 400)
			mode = 0;
	}
	if (picData->Height == 200 || picData->Height == 400)
		mode |= SMODE_BOLD;
	if (mode > -1)
	{
		//mode |= (picData->BitDepth == 8) ? SMODE_BMP2 : SMODE_BMP1;
		//REG_SCREENMODE = (uint8_t)mode;
		if (picData->BitDepth == 8)
			SetBitmapMode256(mode);
		else
			SetBitmapMode16(mode);
	}
	int32_t colors = (picData->BitDepth == 8) ? 256 : 16;
	int8_t* source = (int8_t*)picData;
	source += picData->ImageOffset;
	if (picData->Flags & 1)
		RleUnpack((void*)MEM_VRAM, source, picData->Stride * picData->Height);
	else
		DmaCopy((void*)MEM_VRAM, source, picData->ByteSize, DMA_INT);
	DmaCopy((void*)PALETTE, (int8_t*)((int32_t)picData + picData->ColorOffset), colors * 1, DMA_SHORT);
}

#define FADESPEED 1

void FadeToBlack()
{
	for (int32_t i = 0; i < 32; i += FADESPEED)
	{
		REG_SCREENFADE = i;
		WaitForVBlank();
	}
	REG_SCREENFADE = 31;
}

void FadeFromBlack()
{
	for (int32_t i = 31; i >= 0; i -= FADESPEED)
	{
		REG_SCREENFADE = i;
		WaitForVBlank();
	}
	REG_SCREENFADE = 0;
}

void FadeToWhite()
{
	for (int32_t i = 0; i < 32; i += FADESPEED)
	{
		REG_SCREENFADE = 0x80 | i;
		WaitForVBlank();
	}
	REG_SCREENFADE = 0x80 | 31;
}

void FadeFromWhite()
{
	for (int32_t i = 31; i >= 0; i -= FADESPEED)
	{
		REG_SCREENFADE = 0x80 | i;
		WaitForVBlank();
	}
	REG_SCREENFADE = 0x80;
}

void DrawString(const char* str, int32_t x, int32_t y, int32_t color)
{
	if (interface->DrawChar == NULL) return;
	intoff();
	int32_t sx = x;
	while(*str)
	{
		if (*str == '\n')
		{
			str++;
			x = sx;
			y += (int)(interface->DrawCharHeight & 0xFF00) >> 8;
			continue;
		}
		//TODO: support \t?
		DrawChar(*str++, x, y, color);
		x += 8;
	}
	inton();
}

void DrawFormat(const char* format, int32_t x, int32_t y, int32_t color, ...)
{
	if (interface->DrawChar == NULL) return;
	intoff();
	char buffer[1024];
	va_list args;
	va_start(args, format);
	vsprintf(buffer, format, args);
	char *b = buffer;
	while(*b)
	{
		DrawChar(*b++, x, y, color);
		x += 8;
	}
	va_end(args);
	inton();
}

void DrawChar(char c, int32_t x, int32_t y, int32_t color)
{
	interface->DrawChar(c, x, y, color);
}

