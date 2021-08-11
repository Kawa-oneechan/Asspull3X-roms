#include "../ass.h"
extern IBios* interface;

extern const TImageFile bmp320x200x4;
extern const TImageFile bmp320x240x4;
extern const TImageFile bmp640x480x4;
extern const TImageFile bmp320x200x8;
extern const TImageFile bmp320x240x8;
extern const TImageFile bmp640x480x8;

void WaitForKeySilent()
{
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

void BitmapTest()
{
	DRAW->DisplayPicture((TImageFile*)&bmp320x200x4);
	WaitForKeySilent();
	DRAW->DisplayPicture((TImageFile*)&bmp320x240x4);
	WaitForKeySilent();
	DRAW->DisplayPicture((TImageFile*)&bmp640x480x4);
	WaitForKeySilent();
	DRAW->DisplayPicture((TImageFile*)&bmp320x200x8);
	WaitForKeySilent();
	DRAW->DisplayPicture((TImageFile*)&bmp320x240x8);
	WaitForKeySilent();
	DRAW->DisplayPicture((TImageFile*)&bmp640x480x8);
	WaitForKeySilent();
//	DRAW->ResetPalette();
}
