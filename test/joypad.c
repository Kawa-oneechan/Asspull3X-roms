#include "../ass.h"
IBios* interface;

extern const uint16_t pointerTiles[], pointerPal[];
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
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&pointerTiles, 0x400, DMA_SHORT);
	MISC->DmaCopy(PALETTE + 256, pointerPal, 16, DMA_SHORT);
	OBJECTS_A[0] = OBJECTA_BUILD(260, 0, 1, 0);
	OBJECTS_B[0] = OBJECTB_BUILD(320, 160, 0, 0, 0, 0, 1, 0);

	OBJECTS_B[1] = OBJECTB_BUILD(216 + 16, 32, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[2] = OBJECTB_BUILD(216 + 32, 24, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[3] = OBJECTB_BUILD(216 +  0, 24, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[4] = OBJECTB_BUILD(216 + 16, 16, 0, 0, 0, 0, 1, 0);

	OBJECTS_B[5] = OBJECTB_BUILD(160 + 16, 32, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[6] = OBJECTB_BUILD(160 + 32, 24, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[7] = OBJECTB_BUILD(160 +  0, 24, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[8] = OBJECTB_BUILD(160 + 16, 16, 0, 0, 0, 0, 1, 0);

	//OBJECTS_B[5] = OBJECTB_BUILD(160 + (i * 18), 24, 0, 0, 0, 0, 1, 0);

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

		OBJECTS_B[0] = OBJECTB_BUILD(320 + (axis1 / 1) - 8, 120 + (axis2 / 2) - 8, 0, 0, 0, 0, 1, 0);

		OBJECTS_A[1] = OBJECTA_BUILD(264, (buttons & 0x10) ? 0 : 1, 1, 0);
		OBJECTS_A[2] = OBJECTA_BUILD(268, (buttons & 0x20) ? 0 : 1, 1, 0);
		OBJECTS_A[3] = OBJECTA_BUILD(272, (buttons & 0x40) ? 0 : 1, 1, 0);
		OBJECTS_A[4] = OBJECTA_BUILD(276, (buttons & 0x80) ? 0 : 1, 1, 0);
		OBJECTS_A[8] = OBJECTA_BUILD(280, (buttons & 0x01) ? 0 : 1, 1, 0);
		OBJECTS_A[6] = OBJECTA_BUILD(284, (buttons & 0x02) ? 0 : 1, 1, 0);
		OBJECTS_A[5] = OBJECTA_BUILD(288, (buttons & 0x04) ? 0 : 1, 1, 0);
		OBJECTS_A[7] = OBJECTA_BUILD(292, (buttons & 0x08) ? 0 : 1, 1, 0);

		vbl();
	}
	OBJECTS_A[0] = 0; //hide the cursor
}
