#include "../ass.h"
extern IBios* interface;

extern const uint16_t pointerTiles[];
extern const TImageFile bmp320x240x4;

#define SPRITEA_BUILD(t,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((t) & 0x1FF) << 0)		\
)
#define SPRITEB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
(												\
	(((pr) & 3) << 30) |						\
	(((ds) & 1) << 28) |						\
	(((vf) & 1) << 27) |						\
	(((hf) & 1) << 26) |						\
	(((dh) & 1) << 25) |						\
	(((dw) & 1) << 24) |						\
	(((vp) & 0x7FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)

void MouseTest()
{
	TEXT->ClearScreen();
	TEXT->SetTextColor(0, 7);
	TEXT->Write("Mouse test\nPress any key when satisfied.");
	int oldX = -1, oldY = -1, oldButts = -1;
	int newX, newY, rawX, rawY, buttons;
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0)
	{
		rawX = REG_MOUSE & 0x3FF;
		rawY = (REG_MOUSE >> 16) & 0x1FF;
		buttons = REG_MOUSE >> 30;
		if (rawX != oldX || rawY != oldY || buttons != oldButts)
		{
			TEXT->SetCursorPosition(0, 5);
			printf("0x%08X = %dx%d, %d        \n", REG_MOUSE, rawX, rawY, buttons);
			((int8_t*)MEM_VRAM)[(((newY * 80) + newX) * 2) + 1] = 0x07;
			newX = rawX / 8;
			newY = rawY / 16;
			oldX = rawX;
			oldY = rawY;
			oldButts = buttons;
			((int8_t*)MEM_VRAM)[(((newY * 80) + newX) * 2) + 1] = 0x70;
		}
		vbl();
	}
	while (REG_KEYIN != 0) { vbl(); }
	DRAW->DisplayPicture((TImageFile*)&bmp320x240x4);
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&pointerTiles, 0x2E0, DMA_INT);
	SPRITES_A[0] = SPRITEA_BUILD(256, 1, 0);
	while (REG_KEYIN == 0)
	{
		rawX = REG_MOUSE & 0x3FF;
		rawY = (REG_MOUSE >> 16) & 0x1FF;
		buttons = REG_MOUSE >> 30;
		if (rawX != oldX || rawY != oldY || buttons != oldButts)
		{
			newX = rawX;
			newY = rawY;
			oldX = rawX;
			oldY = rawY;
			oldButts = buttons;
			SPRITES_B[0] = SPRITEB_BUILD(newX, newY, 0, 0, 0, 0, 1, 0);
		}
		vbl();
	}
	while (REG_KEYIN != 0) { vbl(); }
	DRAW->ResetPalette();
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->ClearScreen();
}
