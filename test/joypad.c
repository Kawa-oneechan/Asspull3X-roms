#include "../ass.h"
IBios* interface;

extern const uint16_t spritesTiles[], spritesPal[];
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
	MISC->SetBitmapMode16(SMODE_320 | SMODE_240);
	MISC->DmaClear(BITMAP, 0, 160 * 240, DMA_BYTE);

	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&spritesTiles, 0x480, DMA_SHORT);
	MISC->DmaCopy(PALETTE + 256, spritesPal, 16, DMA_SHORT);

	OBJECTS_B[0] = OBJECTB_BUILD(320, 160, 0, 0, 0, 0, 1, 0);

	OBJECTS_B[1] = OBJECTB_BUILD(200, 104, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[2] = OBJECTB_BUILD(216,  88, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[3] = OBJECTB_BUILD(184,  88, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[4] = OBJECTB_BUILD(200,  72, 0, 0, 0, 0, 1, 0);

	OBJECTS_B[5] = OBJECTB_BUILD(104, 104, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[6] = OBJECTB_BUILD(120,  88, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[7] = OBJECTB_BUILD( 88,  88, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[8] = OBJECTB_BUILD(104,  72, 0, 0, 0, 0, 1, 0);

	OBJECTS_B[9] = OBJECTB_BUILD( 88,  48, 1, 0, 0, 0, 1, 0);
	OBJECTS_B[10]= OBJECTB_BUILD(200,  48, 1, 0, 0, 0, 1, 0);
	OBJECTS_B[11]= OBJECTB_BUILD(144,  80, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[12]= OBJECTB_BUILD(160,  80, 0, 0, 0, 0, 1, 0);

	OBJECTS_A[13]= OBJECTA_BUILD(320,  9, 1, 0);
	OBJECTS_A[14]= OBJECTA_BUILD(324,  9, 1, 0);
	OBJECTS_B[13]= OBJECTB_BUILD(144,  96, 0, 0, 0, 0, 1, 0);
	OBJECTS_B[14]= OBJECTB_BUILD(160,  96, 0, 0, 0, 0, 1, 0);

	const char * const types[] = { "none", "digital only", "analog" };
	char buffer[256] = { 0 };

	int oldStates = -1;
	int oldButtons = -1;
	char oldAxis[2] = { -1, -1 };
	while (INP_KEYIN != 1)
	{
		int buttons = INP_JOYPAD1;
		char axis1 = INP_JOYSTK1H;
		char axis2 = INP_JOYSTK1V;

		if (oldStates != INP_JOYSTATES)
		{
			MISC->DmaClear(BITMAP, 0, 160 * 16, DMA_BYTE);
			TEXT->Format(buffer, "1. %s\n2. %s", types[INP_JOYSTATES & 0x0F], types[(INP_JOYSTATES >> 4) & 0x0F]);
			DRAW->DrawString(buffer, 0, 0, 15);
			OBJECTS_A[0] = OBJECTA_BUILD(260, 0, (INP_JOYSTATES & 0x0F) > 1, 0);
			oldStates = INP_JOYSTATES;
		}

		if (buttons != oldButtons || axis1 != oldAxis[0] || axis2 != oldAxis[1])
		{
			MISC->DmaClear(BITMAP + (160 * 24), 0, 160 * 8, DMA_BYTE);
			TEXT->Format(buffer, "0x%04X %3d,%3d  ", buttons, axis1, axis2);
			DRAW->DrawString(buffer, 0, 24, 15);
			oldButtons = buttons;
			oldAxis[0] = axis1;
			oldAxis[1] = axis2;
		}

		OBJECTS_B[0] = OBJECTB_BUILD(160 + (axis1 / 2) - 8, 120 + (axis2 / 2) - 8, 0, 0, 0, 0, 1, 0);

		OBJECTS_A[1] = OBJECTA_BUILD(264, (buttons & 0x10) ? 0 : 1, 1, 0);
		OBJECTS_A[2] = OBJECTA_BUILD(268, (buttons & 0x20) ? 0 : 1, 1, 0);
		OBJECTS_A[3] = OBJECTA_BUILD(272, (buttons & 0x40) ? 0 : 1, 1, 0);
		OBJECTS_A[4] = OBJECTA_BUILD(276, (buttons & 0x80) ? 0 : 1, 1, 0);
		OBJECTS_A[8] = OBJECTA_BUILD(280, (buttons & 0x01) ? 0 : 1, 1, 0);
		OBJECTS_A[6] = OBJECTA_BUILD(284, (buttons & 0x02) ? 0 : 1, 1, 0);
		OBJECTS_A[5] = OBJECTA_BUILD(288, (buttons & 0x04) ? 0 : 1, 1, 0);
		OBJECTS_A[7] = OBJECTA_BUILD(292, (buttons & 0x08) ? 0 : 1, 1, 0);

		OBJECTS_A[9] = OBJECTA_BUILD(296, (buttons & 0x100) ? 0 : 1, 1, 0);
		OBJECTS_A[10]= OBJECTA_BUILD(304, (buttons & 0x200) ? 0 : 1, 1, 0);
		OBJECTS_A[11]= OBJECTA_BUILD(312, (buttons & 0x400) ? 0 : 1, 1, 0);
		OBJECTS_A[12]= OBJECTA_BUILD(316, (buttons & 0x800) ? 0 : 1, 1, 0);

		vbl();
	}
	MISC->DmaClear(OBJECTS_A, 0, 512, DMA_BYTE);
}
