#include "../ass.h"
extern IBios* interface;

extern const TImageFile bmp320x200x4;
extern const TImageFile bmp320x240x4;
extern const TImageFile bmp640x480x4;
extern const TImageFile bmp320x200x8;
extern const TImageFile bmp320x240x8;
extern const TImageFile bmp640x480x8;
const TImageFile * const bitmaps[] =
{
	&bmp320x200x4, &bmp320x240x4, &bmp640x480x4,
	&bmp320x200x8, &bmp320x240x8, &bmp640x480x8,
};

static void WaitForKey()
{
	DRAW->DrawString("Press any key to continue.", 0, 0, 15);
	while (INP_KEYIN == 0) { vbl(); }
}

void FloodFill(int x, int y, int newColor, uint8_t* dest);

void GraphicsTest()
{
	interface->DrawCharFont -= 0x800;
	for (int i = 0; i < 6; i++)
	{
		DRAW->DisplayPicture((TImageFile*)bitmaps[i]);
		WaitForKey();
	}
	MISC->SetBitmapMode16(SMODE_320 | SMODE_240);
	MISC->DmaClear(BITMAP, 0, 320*240, DMA_BYTE);
	FloodFill(4, 4, 1, BITMAP);
	for (int i = 1; i < 18; i++)
	{
		DRAW->DrawLine(i * 8, 8, i * 8, 232, 0, BITMAP);
		DRAW->DrawLine(8, i * 8, 310, i * 10, 0, BITMAP);
	}
	for (int i = 1; i < 17; i++)
	{
		FloodFill(12, (i * 8) + 2, i - 1, BITMAP);
	}
	FloodFill(4, 4, 8, BITMAP);
	WaitForKey();
}




#define _FFSTACKMAX 2048
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

static int _getPixel4(int x, int y, int stride, uint8_t* dest)
{
	char now = dest[(y * stride) + (x / 2)];
	if (x % 2 == 0)
		return (now & 0x0F);
	return (now & 0xF0) >> 4;
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

void FloodFill(int x, int y, int newColor, uint8_t* dest)
{
	int stride = 640;
	if (REG_SCREENMODE & SMODE_320) stride = 320;
	int width = stride;
	if (REG_SCREENMODE & SMODE_BMP16) stride /= 2;

	int height = 480;
	if (REG_SCREENMODE & SMODE_200) height = 400;
	if (REG_SCREENMODE & SMODE_240) height /= 2;

	int oldColor = _getPixel4(x, y, stride, dest);

	//https://lodev.org/cgtutor/floodfill.html

	if (oldColor == newColor)
		return;
	if (_getPixel4(x, y, stride, dest) != oldColor)
		return;

	_ffStack = malloc(_FFSTACKMAX * 2);
	_ffSP = -1;
	_ffPush(x); _ffPush(y);

	int spanAbove, spanBelow;
	while (!_ffEmpty())
	{
		y = _ffPop();
		x = _ffPop();
		int x1 = x;
		while (x1 >= 0 && _getPixel4(x1, y, stride, dest) == oldColor) x1--;
		x1++;
		spanAbove = spanBelow = 0;
		while (x1 < width && _getPixel4(x1, y, stride, dest) == oldColor)
		{
			_setPixel4(x1, y, stride, newColor, dest);
			if (!spanAbove && y > 0 && _getPixel4(x1, y - 1, stride, dest) == oldColor)
			{
				_ffPush(x1); _ffPush(y - 1);
				spanAbove = 1;
			}
			else if (spanAbove && y > 0 && _getPixel4(x1, y - 1, stride, dest) != oldColor)
			{
				spanAbove = 0;
			}
			if (!spanBelow && y < height - 1 && _getPixel4(x1, y + 1, stride, dest) == oldColor)
			{
				_ffPush(x1); _ffPush(y + 1);
				spanBelow = 1;
			}
			else if (spanBelow && y < height - 1 && _getPixel4(x1, y + 1, stride, dest) != oldColor)
			{
				spanBelow = 0;
			}
			x1++;
		}
	}

	free(_ffStack);
}
