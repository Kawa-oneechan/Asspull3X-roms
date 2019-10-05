#include "../ass.h"
IBios* interface;

#define MAPWIDTH 130
#define SCREENWIDTH 64

extern const uint16_t tilesetPal[], tilesetTiles[], hdma1[];
extern const uint16_t bg1Map[], levelMap[];

void DrawStripe(int source, int target)
{
	uint16_t *s1 = (uint16_t*)&bg1Map[source];
	uint16_t *d1 = &(MAP1[target]);
	uint16_t *s2 = (uint16_t*)&levelMap[source];
	uint16_t *d2 = &(MAP2[target]);
	for (int i = 0; i < 34; i++)
	{
		*d1 = *s1;
		*d2 = *s2;
		s1 += MAPWIDTH;
		s2 += MAPWIDTH;
		d1 += SCREENWIDTH;
		d2 += SCREENWIDTH;
	}
}

int main(void)
{
	interface = (IBios*)(0x01000000);
	REG_SCREENMODE = SMODE_TILE | SMODE_320 | SMODE_240;
	REG_SCREENFADE = 0;

	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	MISC->DmaCopy((int8_t*)0x0E080000, (int8_t*)&tilesetTiles, 2560, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&tilesetPal, 64, DMA_SHORT);

	REG_MAPSET1 = 0x80; //just enable it, don't worry about tile offsets.
	REG_MAPSET2 = 0x80;

	for (int i = 0; i < SCREENWIDTH; i++)
		DrawStripe(i, i);

	REG_SCROLLY1 = 32;
	REG_SCROLLY2 = 16;
	int scroll = 0;
	int col = 40;
	while(1)
	{
		vbl();
		REG_SCROLLX1 = scroll;
		REG_SCROLLX2 = scroll;
		scroll += 1;
		if (scroll % 8 == 0)
		{
			col++;
			DrawStripe(col % MAPWIDTH, col % SCREENWIDTH);
		}
	}
}
