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

void WaitForKey2()
{
	DRAW->DrawString("Press any key to continue.", 0, 0, 15);
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

void BitmapTest()
{
	interface->DrawCharFont -= 0x800;
	for (int i = 0; i < 6; i++)
	{
		DRAW->DisplayPicture((TImageFile*)bitmaps[i]);
		WaitForKey2();
	}
	DRAW->ResetPalette();
}
