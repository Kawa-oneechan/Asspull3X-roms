#include "../ass.h"
#include "funcs.h"

extern int vsprintf(char*, const char*, va_list);

const IDrawingLibrary drawingLibrary =
{
	ResetPalette, DisplayPicture,
	Fade, SetupDrawChar,
	DrawString, DrawFormat,
	DrawChar, DrawLine, FloodFill
};

/* CONSIDER: We have four different copies of this routine after preprocessing.
 * The only difference between them is that two of them work on 4bpp data.
 * One idea might be to have two actual DrawChar functions, one for each bitdepth,
 * and a quick check on what the current screen mode is to decide on the width.
 */

#define DRAWCHAR4(WIDTH) \
	char* glyph = interface->drawCharFont + (c * (interface->drawCharHeight & 0x00FF)); \
	char* target = (char*)MEM_VRAM + (y * (WIDTH/2)) + (x / 2); \
	if (x % 2 == 0) { for (int line = 0; line < (interface->drawCharHeight & 0x00FF); line++) { \
			char g = *glyph++; for (int bit = 0; bit < 8; bit += 2) { \
				int p = g >> bit; \
				if (p & 1) *target = (*target & 0xF0) | (color << 0); \
				if (p & 2) *target = (*target & 0x0F) | (color << 4); \
				target++; } target += (WIDTH/2) - 4; \
		} }	else { for (int line = 0; line < (interface->drawCharHeight & 0x00FF); line++) { \
			char g = *glyph++; if (g & 1) *target = (*target & 0x0F) | (color << 4); \
			for (int bit = 1; bit < 7; bit += 2) { target++; int p = g >> bit; \
				if (p & 1) *target = (*target & 0xF0) | (color << 0); \
				if (p & 2) *target = (*target & 0x0F) | (color << 4); \
			} target++; if ((g >> 7) & 1) *target = (*target & 0x0F) | (color << 4); \
			target += (WIDTH/2) - 4; } } \
	return 8;
#define DRAWCHAR8(WIDTH) \
	char* glyph = interface->drawCharFont + (c * (interface->drawCharHeight & 0x00FF)); \
	char* target = (char*)MEM_VRAM + (y * (WIDTH)) + x; \
	for (int line = 0; line < (interface->drawCharHeight & 0x00FF); line++) { for (int bit = 0; bit < 8; bit++) { \
		int pixel = (*glyph >> bit) & 1; if (pixel == 0) continue; \
		target[bit] = color; \
	}  glyph++; target += (WIDTH); } \
	return 8;

int DrawChar4_320(unsigned char c, int x, int y, int color) { DRAWCHAR4(320) }
int DrawChar4_640(unsigned char c, int x, int y, int color) { DRAWCHAR4(640) }
int DrawChar8_320(unsigned char c, int x, int y, int color) { DRAWCHAR8(320) }
int DrawChar8_640(unsigned char c, int x, int y, int color) { DRAWCHAR8(640) }

void ResetPalette()
{
	static const uint16_t palette[] = {
#if 1
		//Canonical
		0x0000, 0x5400, 0x02A0, 0x56A0, 0x0015, 0x5415, 0x0115, 0x56B5,
		0x294A, 0x7D4A, 0x2BEA, 0x7FEA, 0x295F, 0x7D5F, 0x2BFF, 0x7FFF,
#else
		//Accurate - https://int10h.org/blog/2022/06/ibm-5153-color-true-cga-palette/
		0x0000, 0x6000, 0x0300, 0x6300, 0x0018, 0x6018, 0x01F8, 0x6318,
		0x2529, 0x6D29, 0x2769, 0x7BC9, 0x253B, 0x793E, 0x27DE, 0x7FFF,
#endif
	};
	for (unsigned int idx = 0; idx < array_size(palette); idx++)
	{
		PALETTE[idx] = palette[idx];
	}
}

void DisplayPicture(const TPicFile* picData)
{
	int mode = 0;
	if (picData->width == 320)
		mode |= SMODE_320;
	if (picData->height == 240 || picData->height == 200)
		mode |= SMODE_240;
	if (picData->height == 200 || picData->height == 400)
		mode |= SMODE_1610;
	if (picData->bitDepth == 8)
		REG_SCREENMODE = SMODE_BMP256 | mode;
	else
		REG_SCREENMODE = SMODE_BMP16 | mode;
	int colors = (picData->bitDepth == 8) ? 256 : 16;
	if (!(picData->flags & PIC_NOPAL))
		DmaCopy((void*)PALETTE, (int8_t*)((int)picData + picData->colorOffset), colors * 1, DMA_SHORT);
	int8_t* source = (int8_t*)picData;
	source += picData->imageOffset;
	if (picData->flags & PIC_RLE)
		RleUnpack((void*)MEM_VRAM, source, picData->stride * picData->height);
	else
		DmaCopy((void*)MEM_VRAM, source, picData->byteSize, DMA_INT);

	if (picData->flags & PIC_HDMA)
	{
		uint8_t* hdma = (uint8_t*)((int)picData + picData->hdmaOffset);
		uint8_t channels = *hdma;
		hdma++;
		int32_t p = (int32_t)PALETTE;
		for (int i = 0; i < channels; i++)
		{
			uint16_t size = *(uint16_t*)hdma;
			hdma += 2;
			uint32_t ctrl = *(uint32_t*)hdma;
			hdma += 4;
			int32_t here = (int32_t)hdma;
			REG_HDMASOURCE[i] = here;
			REG_HDMATARGET[i] = p;
			REG_HDMACONTROL[i] = ctrl;
			hdma += size;
			p += 2;
		}
	}
}

#define FADESPEED 1

void Fade(bool in, bool toWhite)
{
	char white = toWhite ? 0x80 : 0x00;
	if (!in)
	{
		for (int i = 0; i < 32; i += FADESPEED)
		{
			REG_SCREENFADE = i | white;
			WaitForVBlank();
		}
		REG_SCREENFADE = 31;
	}
	else
	{
		for (int i = 31; i >= 0; i -= FADESPEED)
		{
			REG_SCREENFADE = i | white;
			WaitForVBlank();
		}
		REG_SCREENFADE = 0;
	}
}

void SetupDrawChar(int(*dcCallback)(unsigned char, int, int, int))
{
	if (dcCallback)
		interface->drawChar = dcCallback;
	else
	{
		if (REG_SCREENMODE & SMODE_BMP16)
			interface->drawChar = (REG_SCREENMODE & SMODE_320) ? DrawChar4_320 : DrawChar4_640;
		else if (REG_SCREENMODE & SMODE_BMP256)
			interface->drawChar = (REG_SCREENMODE & SMODE_320) ? DrawChar8_320 : DrawChar8_640;
		else
			interface->drawChar = NULL;
	}
}

void DrawString(const char* str, int x, int y, int color)
{
	if (interface->drawChar == NULL) return;
	int sx = x;
	while(*str)
	{
		if (*str == '\n')
		{
			str++;
			x = sx;
			y += (int)(interface->drawCharHeight & 0xFF00) >> 8;
			continue;
		}
		//TODO: support \t?
		x += DrawChar(*str++, x, y, color);
	}
}

void DrawFormat(const char* format, int x, int y, int color, ...)
{
	if (interface->drawChar == NULL) return;
	char buffer[1024];
	va_list args;
	va_start(args, color);
	vsprintf(buffer, format, args);
	va_end(args);
	DrawString(buffer, x, y, color);
}

int DrawChar(char ch, int x, int y, int color)
{
	if (interface->drawChar == NULL) return 8;
	return interface->drawChar(ch, x, y, color);
}

extern int abs(int);

static int _getPixel8(int x, int y, int stride, uint8_t* dest)
{
	return dest[(y * stride) + x];
}

static int _getPixel4(int x, int y, int stride, uint8_t* dest)
{
	char now = dest[(y * stride) + (x / 2)];
	if (x % 2 == 0)
		return (now & 0x0F);
	return (now & 0xF0) >> 4;
}

static void _setPixel8(int x, int y, int stride, int color, uint8_t* dest)
{
	dest[(y * stride) + x] = color;
}

static void _setPixel4(int x, int y, int stride, int color, uint8_t* dest)
{
	char now = dest[(y * stride) + (x / 2)];
	if (x % 2 == 0)
		now = (now & 0xF0) | (color << 0);
	else
		now = (now & 0x0F) | (color << 4);
	dest[(y * stride) + (x / 2)] = now;
}

#define SWAP(A, B) { int tmp = A; A = B; B = tmp; }

void DrawLine(int x1, int y1, int x2, int y2, int color, uint8_t* dest)
{
	int stride = 640;
	if (REG_SCREENMODE & SMODE_320) stride /= 2;
	if (REG_SCREENMODE & SMODE_BMP16) stride /= 2;

	if (dest == NULL) dest = BITMAP;

	void(*setPixel)(int,int,int,int,uint8_t*) = _setPixel8;
	if (REG_SCREENMODE & SMODE_BMP16)
		setPixel = _setPixel4;

	if (y1 == y2)
	{
		if (x2 < x1)
			SWAP(x2, x1);
		for (int i = x1; i <= x2; i++)
			setPixel(i, y1, stride, color, dest);
		return;
	}
	if (x1 == x2)
	{
		if (y1 > y2)
			SWAP(y1, y2);
		for (int i = y1; i <= y2; i++)
			setPixel(x1, i, stride, color, dest);
		return;
	}

	int dy = y2 - y1;
	int dx = x2 - x1;
	int stepy = dy < 0 ? -1 : 1;
	int stepx = dx < 0 ? -1 : 1;
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
}

#define _FFSTACKMAX 512
static uint16_t* _ffStack;
static int _ffSP = -1;

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

void FloodFill(int x, int y, int newColor, uint8_t* dest)
{
	uint16_t _ffTrueStack[_FFSTACKMAX * 2];

	int stride = 640;
	if (REG_SCREENMODE & SMODE_320) stride = 320;
	int width = stride;
	if (REG_SCREENMODE & SMODE_BMP16) stride /= 2;

	int height = 480;
	if (REG_SCREENMODE & SMODE_1610) height = 400;
	if (REG_SCREENMODE & SMODE_240) height /= 2;

	int(*getPixel)(int,int,int,uint8_t*) = _getPixel8;
	void(*setPixel)(int,int,int,int,uint8_t*) = _setPixel8;
	if (REG_SCREENMODE & SMODE_BMP16)
	{
		getPixel = _getPixel4;
		setPixel = _setPixel4;
	}

	if (dest == NULL) dest = BITMAP;

	int oldColor = getPixel(x, y, stride, dest);

	//https://lodev.org/cgtutor/floodfill.html

	if (oldColor == newColor)
		return;
	if (getPixel(x, y, stride, dest) != oldColor)
		return;

	_ffStack = _ffTrueStack; //malloc(_FFSTACKMAX * 2);
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

	//free(_ffStack);
}
