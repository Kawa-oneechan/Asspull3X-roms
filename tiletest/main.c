#include "../ass.h"
IBios* interface;

#define MAPWIDTH 130
#define SCREENWIDTH 64

extern const uint16_t tilesetPal[], tilesetTiles[], hdma1[];
extern const uint16_t farahPal[], farahTiles[];
extern const uint16_t grassTiles[], questionTiles[];
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

int main(void)
{
	interface = (IBios*)(0x01000000);
	REG_SCREENMODE = SMODE_TILE | SMODE_SPRITES;
	REG_SCREENFADE = 0;

	REG_INTRMODE |= 0x80;

	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	MISC->DmaCopy((int8_t*)0x0E080000, (int8_t*)&tilesetTiles, 2560, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&tilesetPal, 64, DMA_SHORT);

	MISC->DmaCopy((int8_t*)0x0E082000, (int8_t*)&farahTiles, 64, DMA_INT);
	MISC->DmaCopy(PALETTE + 32, (int8_t*)&farahPal, 32, DMA_SHORT);
	*(uint16_t*)0xE108000 = SPRITEA_BUILD(256, 1, 2);
	*(uint32_t*)0xE108200 = SPRITEB_BUILD(152, 176, 0, 1, 0, 0, 1, 1);

	REG_MAPSET1 = 0x80; //just enable it, don't worry about tile offsets.
	REG_MAPSET2 = 0x80;

	for (int i = 0; i < SCREENWIDTH; i++)
		DrawStripe(i, i);

	REG_INTRMODE &= ~0x80;

	REG_SCROLLY1 = 32;
	REG_SCROLLY2 = 16;
	int scroll = 0;
	int col = 40;
	int animation = 0;
	for(;;)
	{
		vbl();
		REG_SCROLLX1 = scroll;
		REG_SCROLLX2 = scroll;
		scroll += 1;
		if (REG_TICKCOUNT % 4 == 0)
		{
			animation++;
			MISC->DmaCopy((int8_t*)0x0E080020, (int8_t*)&questionTiles + ((animation % 9) * 128), 32, DMA_INT);
			MISC->DmaCopy((int8_t*)0x0E080240, (int8_t*)&grassTiles + ((animation % 3) * 192), 48, DMA_INT);
		}
		if (scroll % 8 == 0)
		{
			col++;
			DrawStripe(col % MAPWIDTH, col % SCREENWIDTH);
		}
	}
}
