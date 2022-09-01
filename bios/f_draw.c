#include "../ass.h"
#include "funcs.h"

extern int vsprintf(char*, const char*, va_list);

const IDrawingLibrary drawingLibrary =
{
	ResetPalette, DisplayPicture,
	Fade, DrawString, DrawFormat,
	DrawChar, DrawLine
};

/* CONSIDER: We have four different copies of this routine after preprocessing.
 * The only difference between them is that two of them work on 4bpp data.
 * One idea might be to have two actual DrawChar functions, one for each bitdepth,
 * and a quick check on what the current screen mode is to decide on the width.
 */

#define DRAWCHAR4(WIDTH) \
	char* glyph = interface->DrawCharFont + (c * (interface->DrawCharHeight & 0x00FF)); \
	char* target = (char*)MEM_VRAM + (y * (WIDTH/2)) + (x / 2); \
	if (x % 2 == 0) { for (int line = 0; line < (interface->DrawCharHeight & 0x00FF); line++) { \
			char g = *glyph++; for (int bit = 0; bit < 8; bit += 2) { \
				int p = g >> bit; \
				if (p & 1) *target = (*target & 0xF0) | (color << 0); \
				if (p & 2) *target = (*target & 0x0F) | (color << 4); \
				target++; } target += (WIDTH/2) - 4; \
		} }	else { for (int line = 0; line < (interface->DrawCharHeight & 0x00FF); line++) { \
			char g = *glyph++; if (g & 1) *target = (*target & 0x0F) | (color << 4); \
			for (int bit = 1; bit < 7; bit += 2) { target++; int p = g >> bit; \
				if (p & 1) *target = (*target & 0xF0) | (color << 0); \
				if (p & 2) *target = (*target & 0x0F) | (color << 4); \
			} target++; if ((g >> 7) & 1) *target = (*target & 0x0F) | (color << 4); \
			target += (WIDTH/2) - 4; } }
#define DRAWCHAR8(WIDTH) \
	char* glyph = interface->DrawCharFont + (c * (interface->DrawCharHeight & 0x00FF)); \
	char* target = (char*)MEM_VRAM + (y * (WIDTH)) + x; \
	for (int line = 0; line < (interface->DrawCharHeight & 0x00FF); line++) { for (int bit = 0; bit < 8; bit++) { \
			int pixel = (*glyph >> bit) & 1; if (pixel == 0) continue; \
			target[bit] = color; \
		}  glyph++; target += (WIDTH); }

void DrawChar4_320(unsigned char c, int x, int y, int color) { DRAWCHAR4(320) }
void DrawChar4_640(unsigned char c, int x, int y, int color) { DRAWCHAR4(640) }
void DrawChar8_320(unsigned char c, int x, int y, int color) { DRAWCHAR8(320) }
void DrawChar8_640(unsigned char c, int x, int y, int color) { DRAWCHAR8(640) }

static const uint16_t palette[] = {
	0x0000, 0x5400, 0x02A0, 0x56A0, 0x0015, 0x5415, 0x0115, 0x56B5,
	0x294A, 0x7D4A, 0x2BEA, 0x7FEA, 0x295F, 0x7D5F, 0x2BFF, 0x7FFF,
};


void ResetPalette()
{
	for (int idx = 0; idx < 16; idx++)
	{
		PALETTE[idx] = palette[idx];
	}
}

void DisplayPicture(TImageFile* picData)
{
	int mode = -1;
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
	int colors = (picData->BitDepth == 8) ? 256 : 16;
	DmaCopy((void*)PALETTE, (int8_t*)((int)picData + picData->ColorOffset), colors * 1, DMA_SHORT);
	int8_t* source = (int8_t*)picData;
	source += picData->ImageOffset;
	if (picData->Flags & 1)
		RleUnpack((void*)MEM_VRAM, source, picData->Stride * picData->Height);
	else
		DmaCopy((void*)MEM_VRAM, source, picData->ByteSize, DMA_INT);

	if (picData->Flags & 2) //HDMA
	{
		uint8_t* hdma = (uint8_t*)((int)picData + picData->HdmaOffset);
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

void DrawString(const char* str, int x, int y, int color)
{
	if (interface->DrawChar == NULL) return;
	int sx = x;
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
}

void DrawFormat(const char* format, int x, int y, int color, ...)
{
	if (interface->DrawChar == NULL) return;
	char buffer[1024];
	va_list args;
	va_start(args, color);
	vsprintf(buffer, format, args);
	char *b = buffer;
	while(*b)
	{
		DrawChar(*b++, x, y, color);
		x += 8;
	}
	va_end(args);
}

void DrawChar(char ch, int x, int y, int color)
{
	interface->DrawChar(ch, x, y, color);
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
