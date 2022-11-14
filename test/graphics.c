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
	DRAW->SetupDrawChar(0);
	DRAW->DrawString("Press any key to continue.", 0, 0, 15);
	while (INP_KEYIN == 0) { vbl(); }
}

void FloodFill(int x, int y, int newColor, uint8_t* dest);

void GraphicsTest()
{
	interface->drawCharFont = (char*)TEXTFONT_THIN8;
	for (int i = 0; i < 6; i++)
	{
		DRAW->DisplayPicture((TImageFile*)bitmaps[i]);
		WaitForKey();
	}
	REG_SCREENMODE = SMODE_BMP16 | SMODE_320 | SMODE_240;
	MISC->DmaClear(BITMAP, 0, 320*240, DMA_BYTE);
	DRAW->FloodFill(4, 4, 1, BITMAP);
	for (int i = 1; i < 18; i++)
	{
		DRAW->DrawLine(i * 8, 8, i * 8, 232, 0, BITMAP);
		DRAW->DrawLine(8, i * 8, 310, i * 10, 0, BITMAP);
	}
	for (int i = 1; i < 17; i++)
	{
		DRAW->FloodFill(12, (i * 8) + 2, i - 1, BITMAP);
	}
	DRAW->FloodFill(4, 4, 8, BITMAP);
	WaitForKey();
}
