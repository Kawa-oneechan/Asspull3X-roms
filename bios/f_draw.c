#include "../ass.h"
#include "funcs.h"

extern int32_t vsprintf(char*, const char*, va_list);

const IDrawingLibrary drawingLibrary =
{
	ResetPalette, DisplayPicture,
	FadeToBlack, FadeFromBlack,
	FadeToWhite, FadeFromWhite,
	DrawString, DrawFormat, DrawChar,
	DrawLine, FloodFill
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
				if (p & 1) *target = (*target & 0xF0) | (color << 0); \
				if (p & 2) *target = (*target & 0x0F) | (color << 4); \
				target++; } target += (WIDTH/2) - 4; \
		} }	else { for (int32_t line = 0; line < (interface->DrawCharHeight & 0x00FF); line++) { \
			char g = *glyph++; if (g & 1) *target = (*target & 0x0F) | (color << 4); \
			for (int32_t bit = 1; bit < 7; bit += 2) { target++; int32_t p = g >> bit; \
				if (p & 1) *target = (*target & 0xF0) | (color << 0); \
				if (p & 2) *target = (*target & 0x0F) | (color << 4); \
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
	0x0000, 0x5400, 0x02A0, 0x56A0, 0x0015, 0x5415, 0x0115, 0x56B5,
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
		//mode |= (picData->BitDepth == 8) ? SMODE_BMP256 : SMODE_BMP16;
		//REG_SCREENMODE = (uint8_t)mode;
		if (picData->BitDepth == 8)
			SetBitmapMode256(mode);
		else
			SetBitmapMode16(mode);
	}
	int32_t colors = (picData->BitDepth == 8) ? 256 : 16;
	DmaCopy((void*)PALETTE, (int8_t*)((int32_t)picData + picData->ColorOffset), colors * 1, DMA_SHORT);
	int8_t* source = (int8_t*)picData;
	source += picData->ImageOffset;
	if (picData->Flags & 1)
		RleUnpack((void*)MEM_VRAM, source, picData->Stride * picData->Height);
	else
		DmaCopy((void*)MEM_VRAM, source, picData->ByteSize, DMA_INT);
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

void DrawChar(char ch, int32_t x, int32_t y, int32_t color)
{
	interface->DrawChar(ch, x, y, color);
}

extern int abs(int);

static int32_t _getPixel8(int32_t x, int32_t y, int32_t stride, uint8_t* dest)
{
	return dest[(y * stride) + x];
}

static int32_t _getPixel4(int32_t x, int32_t y, int32_t stride, uint8_t* dest)
{
	char now = dest[(y * stride) + (x / 2)];
	if (x % 2 == 0)
		return (now & 0x0F);
	return (now & 0xF0) >> 4;
}

static void _setPixel8(int32_t x, int32_t y, int32_t stride, int32_t color, uint8_t* dest)
{
	dest[(y * stride) + x] = color;
}

static void _setPixel4(int32_t x, int32_t y, int32_t stride, int32_t color, uint8_t* dest)
{
	char now = dest[(y * stride) + (x / 2)];
	if (x % 2 == 0)
		now = (now & 0xF0) | (color << 0);
	else
		now = (now & 0x0F) | (color << 4);
	dest[(y * stride) + (x / 2)] = now;
}

#define SWAP(A, B) { int32_t tmp = A; A = B; B = tmp; }

void DrawLine(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t color, uint8_t* dest)
{
	int8_t ints = REG_INTRMODE;
	intoff();

	int stride = 640;
	if (REG_SCREENMODE & SMODE_320) stride /= 2;
	if (REG_SCREENMODE & SMODE_BMP16) stride /= 2;

	void(*setPixel)(int32_t,int32_t,int32_t,int32_t,uint8_t*) = _setPixel8;
	if (REG_SCREENMODE & SMODE_BMP16)
		setPixel = _setPixel4;

	if (y1 == y2)
	{
		if (x2 < x1)
			SWAP(x2, x1);
		for (int i = x1; i <= x2; i++)
			setPixel(i, y1, stride, color, dest);
		REG_INTRMODE = ints;
		return;
	}
	if (x1 == x2)
	{
		if (y1 > y2)
			SWAP(y1, y2);
		for (int i = y1; i <= y2; i++)
			setPixel(x1, i, stride, color, dest);
		REG_INTRMODE = ints;
		return;
	}

	int32_t dy = y2 - y1;
	int32_t dx = x2 - x1;
	int32_t stepy = dy < 0 ? -1 : 1;
	int32_t stepx = dx < 0 ? -1 : 1;
	dy = abs(dy) << 1;
	dx = abs(dx) << 1;

	setPixel(x1, y1, stride, color, dest);
	setPixel(x2, y2, stride, color, dest);

	if (dx > dy)
	{
		int fraction = dy - (dx >> 1);
		while (x1 != x2) {
			if (fraction >= 0)
			{
				y1 += stepy;
				fraction -= dx;
			}
			x1 += stepx;
			fraction += dy;
			setPixel(x1, y1, stride, color, dest);
		}
	}
	else
	{
		int fraction = dx - (dy >> 1);
		while (y1 != y2)
		{
			if (fraction >= 0)
			{
				x1 += stepx;
				fraction -= dy;
			}
			y1 += stepy;
			fraction += dx;
			setPixel(x1, y1, stride, color, dest);
		}
	}

	REG_INTRMODE = ints;
}

#define _FFSTACKMAX 2048
static uint16_t* _ffStack;
static int32_t _ffSP = -1;

static int _ffEmpty()
{
	return (_ffSP == -1);
}

static int _ffFull()
{
	return (_ffSP == _FFSTACKMAX);
}

static inline void _ffPush(int16_t i)
{
	if (!_ffFull())
	{
		_ffSP++;
		_ffStack[_ffSP] = i;
	}
}

static inline int16_t _ffPop()
{
	if (!_ffEmpty())
	{
		int16_t data = _ffStack[_ffSP];
		_ffSP--;
		return data;
	}
	return 0;
}

static inline int16_t _ffPeek()
{
	return _ffStack[_ffSP];
}

void FloodFill(int32_t x, int32_t y, int32_t newColor, uint8_t* dest)
{
	int8_t ints = REG_INTRMODE;
	intoff();

	int stride = 640;
	if (REG_SCREENMODE & SMODE_320) stride = 320;
	int width = stride;
	if (REG_SCREENMODE & SMODE_BMP16) stride /= 2;

	int height = 480;
	if (REG_SCREENMODE & SMODE_200) height = 400;
	if (REG_SCREENMODE & SMODE_240) height /= 2;

	int32_t(*getPixel)(int32_t,int32_t,int32_t,uint8_t*) = _getPixel8;
	void(*setPixel)(int32_t,int32_t,int32_t,int32_t,uint8_t*) = _setPixel8;
	if (REG_SCREENMODE & SMODE_BMP16)
	{
		getPixel = _getPixel4;
		setPixel = _setPixel4;
	}

	int32_t oldColor = getPixel(x, y, stride, dest);

	//https://lodev.org/cgtutor/floodfill.html

	if (oldColor == newColor)
	{
		REG_INTRMODE = ints;
		return;
	}
	if (getPixel(x, y, stride, dest) != oldColor)
	{
		REG_INTRMODE = ints;
		return;
	}

	_ffStack = malloc(_FFSTACKMAX * 2);
	_ffSP = -1;
	_ffPush(x); _ffPush(y);

	int spanAbove, spanBelow;
	while (!_ffEmpty())
	{
		y = _ffPop();
		x = _ffPop();
		int x1 = x;
		while (x1 >= 0 && getPixel(x1, y, stride, dest) == oldColor) x1--;
		x1++;
		spanAbove = spanBelow = 0;
		while (x1 < width && getPixel(x1, y, stride, dest) == oldColor)
		{
			setPixel(x1, y, stride, newColor, dest);
			if (!spanAbove && y > 0 && getPixel(x1, y - 1, stride, dest) == oldColor)
			{
				_ffPush(x1); _ffPush(y - 1);
				spanAbove = 1;
			}
			else if (spanAbove && y > 0 && getPixel(x1, y - 1, stride, dest) != oldColor)
			{
				spanAbove = 0;
			}
			if (!spanBelow && y < height - 1 && getPixel(x1, y + 1, stride, dest) == oldColor)
			{
				_ffPush(x1); _ffPush(y + 1);
				spanBelow = 1;
			}
			else if (spanBelow && y < height - 1 && getPixel(x1, y + 1, stride, dest) != oldColor)
			{
				spanBelow = 0;
			}
			x1++;
		}
	}

	free(_ffStack);

	REG_INTRMODE = ints;
}
