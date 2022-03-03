#include "../ass.h"
extern IBios* interface;

extern const uint16_t pointerTiles[];
extern const TImageFile bmp320x240x4;

#define OBJECTA_BUILD(t,b,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((b) &  3) <<  9) |		\
	(((t) & 0x1FF) << 0)		\
)
#define OBJECTB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
(												\
	(((pr) & 3) << 29) |						\
	(((ds) & 1) << 28) |						\
	(((vf) & 1) << 27) |						\
	(((hf) & 1) << 26) |						\
	(((dh) & 1) << 25) |						\
	(((dw) & 1) << 24) |						\
	(((vp) & 0xFFF) << 12) |					\
	(((hp) & 0xFFF) << 0)						\
)

typedef struct
{
	char changed;
	char buttons;
	short x, y;
	short oldX, oldY;
	char oldButtons;
} TMouseState;
TMouseState MouseState;

void HandleMouse()
{
	short rm = REG_MOUSE;
	int rawX = rm & 0x1F;
	int rawY = (rm >> 7) & 0x1F;
	if (rm & 0x40)   rawX = -rawX;
	if (rm & 0x2000) rawY = -rawY;
	MouseState.buttons = (rm >> 14) & 3;

	MouseState.x += rawX;
	MouseState.y += rawY;
	if (MouseState.x < 0) MouseState.x = 0;
	if (MouseState.y < 0) MouseState.y = 0;
	if (MouseState.x > 319) MouseState.x = 319;
	if (MouseState.y > 239) MouseState.y = 239;

	MouseState.changed = (MouseState.x != MouseState.oldX || MouseState.y != MouseState.oldY || MouseState.buttons != MouseState.oldButtons);

	MouseState.oldX = MouseState.x;
	MouseState.oldY = MouseState.y;
	MouseState.oldButtons = MouseState.buttons;
}

void MouseTest()
{
	TEXT->ClearScreen();
	TEXT->SetTextColor(0, 7);
	TEXT->Write("Mouse test\nPress any key when satisfied.");
	int tX, tY;

	MouseState.x = 160; MouseState.y = 120;

	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0)
	{
		HandleMouse();
		if (MouseState.changed)
		{
			TEXT->SetCursorPosition(0, 5);
			printf("%dx%d, %d     \n", MouseState.x, MouseState.y, MouseState.buttons);
			((int8_t*)MEM_VRAM)[(((tY * 80) + tX) * 2) + 1] = 0x07;
			tX = MouseState.x / 4;
			tY = MouseState.y / 8;
			((int8_t*)MEM_VRAM)[(((tY * 80) + tX) * 2) + 1] = 0x70;
		}
		vbl();
	}

	while (REG_KEYIN != 0) { vbl(); }

	MouseState.x = 136; MouseState.y = 104;
	const short colors[] = { 0x0000, 0x2223, 0x5184, 0x1A9E };

	DRAW->DisplayPicture((TImageFile*)&bmp320x240x4);
	MISC->DmaCopy(PALETTE + 256, PALETTE, 16, DMA_SHORT);
	DRAW->DrawString("Mouse test\nPress any key when satisfied.", 0, 0, 15);
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&pointerTiles, 0x2E0, DMA_INT);
	OBJECTS_A[0] = OBJECTA_BUILD(256, 1, 1, 1);
	OBJECTS_B[0] = OBJECTB_BUILD(MouseState.x + 2, MouseState.y + 2, 0, 0, 0, 0, 1, 0);
	OBJECTS_A[1] = OBJECTA_BUILD(256, 0, 1, 0);
	OBJECTS_B[1] = OBJECTB_BUILD(MouseState.x + 0, MouseState.y + 0, 0, 0, 0, 0, 1, 0);

	while (REG_KEYIN == 0)
	{
		HandleMouse();
		if (MouseState.changed)
		{
			PALETTE[0] = colors[(int)MouseState.buttons];
			OBJECTS_B[0] = OBJECTB_BUILD(MouseState.x + 2, MouseState.y + 2, 0, 0, 0, 0, 1, 0);
			OBJECTS_B[1] = OBJECTB_BUILD(MouseState.x + 0, MouseState.y + 0, 0, 0, 0, 0, 1, 0);
		}
		vbl();
	}
	while (REG_KEYIN != 0) { vbl(); }
	OBJECTS_A[0] = OBJECTS_A[1] = 0; //hide the cursor
	DRAW->ResetPalette();
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->ClearScreen();
}
