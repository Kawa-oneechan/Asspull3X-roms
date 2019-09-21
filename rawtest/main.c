#include "../ass.h"

int kek, kik;

int32_t main(void)
{
	REG_SCREENMODE = SMODE_BMP1 | SMODE_320 | SMODE_240;
	kek = 0;
	kik = 1;
	((int16_t*)MEM_VRAM)[kek] = 0x1111;
	while(1)
	{
//		while(REG_LINE >= 480);
//		while(REG_LINE < 480);
	}
}

void ExHandler()
{
	asm("rte");
}

void VBlankHandler()
{
	((int16_t*)MEM_VRAM)[kek] = 0x1111;
	kek++;
	asm("rte");
}

void HBlankHandler()
{
	kik ^= 1;
	if (kik)
		PALETTE[1] = 0x5400;
	else
		PALETTE[1] = 0x007F;
	asm("rte");
}
