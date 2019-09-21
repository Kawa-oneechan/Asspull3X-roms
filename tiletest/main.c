#include "../ass.h"
IBios* interface;

#define MAP ((uint16_t*)0x0E000000)
#define REG_SCROLLX1 *(uint16_t*)(MEM_IO+0x0010)
#define REG_SCROLLY1 *(uint16_t*)(MEM_IO+0x0012)

extern const uint16_t tileset[256];

int main(void)
{
	interface = (IBios*)(0x01000000);
	REG_SCREENMODE = SMODE_TILE | SMODE_320 | SMODE_240;
	REG_SCREENFADE = 0;
	TEXT->ClearScreen();

	MISC->DmaCopy((int8_t*)0x0E080000, (int8_t*)&tileset, 1024, DMA_INT);
	char* p = "Hello world! 1 2 3 wakka jawakka0123456789012345678901234567HEY!";
	int i = 0;
	while(*p)
	{
		MAP[i] = *p++;
		i++;
	}
	p =       "This is line two... Peter Piper picked a peck of pickled peppers";
	i = 0;
	while(*p)
	{
		MAP[64 + i] = *p++;
		i++;
	}
	for (i = 128; i < 2048; i += 64)
		MAP[i] = 'W';
	MAP[256] = 0x400 | 'b';
	/*
	for (int i = 0; i < 16; i++)
	{
		MAP[i] = 1;
		MAP[i+64] = 2;
		MAP[i+128] = 3;
		//MAP[i+192] = 4;
	}
	*/
	int scroll = 0;
	while(1)
	{
		vbl();
		REG_SCROLLY1 = scroll;
		scroll += 2;
	}
}
