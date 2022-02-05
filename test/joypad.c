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
	(((vp) & 0x7FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)

void JoypadTest()
{
	MISC->SetTextMode(SMODE_320 | SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&pointerTiles, 0x2E0, DMA_INT);
	OBJECTS_A[0] = OBJECTA_BUILD(256, 0, 1, 0);
	OBJECTS_B[0] = OBJECTB_BUILD(320, 160, 0, 0, 0, 0, 1, 0);
	while (1)
	{
		REG_JOYPAD = 1; //reset
		int dpadbuts = REG_JOYPAD & 0xFF; //int with mask to prevent "0xFFFFFF80"
		int extrabuts = REG_JOYPAD & 0xFF;
		char axis1 = REG_JOYPAD;
		char axis2 = REG_JOYPAD;

		TEXT->SetCursorPosition(0, 0);
		printf("0x%02X 0x%02X %3d,%3d  ", dpadbuts, extrabuts, axis1, axis2);

		OBJECTS_B[0] = OBJECTB_BUILD(160 + (axis1 / 2), 120 + (axis2 / 2), 0, 0, 0, 0, 1, 0);

		vbl();
	}
	OBJECTS_A[0] = 0; //hide the cursor
}
