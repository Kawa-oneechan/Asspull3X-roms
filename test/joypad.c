#include "../ass.h"
IBios* interface;

extern const uint16_t pointerTiles[];
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
	(((vp) & 0x3FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)

void JoypadTest()
{
	MISC->SetTextMode(SMODE_320 | SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&pointerTiles, 0x2E0, DMA_INT);
	MISC->DmaCopy(PALETTE + 256, PALETTE, 16, DMA_SHORT);
	OBJECTS_A[0] = OBJECTA_BUILD(256, 0, 1, 0);
	OBJECTS_B[0] = OBJECTB_BUILD(320, 160, 0, 0, 0, 0, 1, 0);

	const char * const types[] = { "none", "digital only", "analog" };
	printf("1. %s\n", types[INP_JOYSTATES & 0x0F]);
	printf("2. %s\n", types[(INP_JOYSTATES >> 4) & 0x0F]);

	while (1)
	{
		int buttons = INP_JOYPAD1;
		char axis1 = INP_JOYSTK1H;
		char axis2 = INP_JOYSTK1V;

		TEXT->SetCursorPosition(0, 3);
		printf("0x%04X %3d,%3d  ", buttons, axis1, axis2);

		OBJECTS_B[0] = OBJECTB_BUILD(160 + (axis1 / 2), 120 + (axis2 / 2), 0, 0, 0, 0, 1, 0);

		vbl();
	}
	OBJECTS_A[0] = 0; //hide the cursor
}
