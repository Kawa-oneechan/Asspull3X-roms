#include "../ass.h"
IBios* interface;

#define MAPWIDTH 130
#define SCREENWIDTH 64

extern const uint16_t tilesetPal[], tilesetTiles[], hdma1[];
extern const uint16_t farahPal[], farahTiles[];
extern const uint16_t grassTiles[], questionTiles[];
extern const uint16_t bg1Map[], bg2Map[], levelMap[];

void DrawStripe(int source, int target)
{
	uint16_t *s1 = (uint16_t*)&bg2Map[source];
	uint16_t *d1 = &(MAP1[target]);
	uint16_t *s2 = (uint16_t*)&bg1Map[source];
	uint16_t *d2 = &(MAP2[target]);
	uint16_t *s3 = (uint16_t*)&levelMap[source];
	uint16_t *d3 = &(MAP3[target]);
	for (int i = 0; i < 34; i++)
	{
		*d1 = *s1;
		*d2 = *s2;
		*d3 = *s3;
		s1 += MAPWIDTH;
		s2 += MAPWIDTH;
		s3 += MAPWIDTH;
		d1 += SCREENWIDTH;
		d2 += SCREENWIDTH;
		d3 += SCREENWIDTH;
	}
}

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

typedef struct
{
	uint16_t palette:4;
	uint16_t enabled:1;
	uint16_t _waste:2;
	uint16_t tile:9;
} TObjectA;
#define objectsA ((TObjectA*)OBJECTS_A)
typedef struct
{
	uint32_t priority:3;
	uint32_t large:1;

	uint32_t flipV:1;
	uint32_t flipH:1;
	uint32_t tall:1;
	uint32_t wide:1;

	uint32_t _waste2:3;
	uint32_t y:9;
	uint32_t _waste3:2;
	uint32_t x:10;
} TObjectB;
#define objectsB ((TObjectB*)OBJECTS_B)

int main(void)
{
	REG_SCREENMODE = SMODE_TILE;
	REG_SCREENFADE = 0;

	intoff();

	MISC->DmaClear(TILESET, 0, 0x4000, DMA_INT);
	MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);

	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	MISC->DmaCopy(TILESET, (int8_t*)&tilesetTiles, 2560, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&tilesetPal, 32, DMA_SHORT);

	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&farahTiles, 64, DMA_INT);
	MISC->DmaCopy(PALETTE + 32, (int8_t*)&farahPal, 16, DMA_SHORT);
	OBJECTS_A[0] = OBJECTA_BUILD(256, 0, 1, 2);
	OBJECTS_B[0] = OBJECTB_BUILD(152, 176, 0, 1, 0, 0, 1, 0);

	REG_MAPSET = 0x70; //just enable it, don't worry about tile offsets.

	for (int i = 0; i < SCREENWIDTH; i++)
		DrawStripe(i, i);

	inton();

	REG_SCROLLY1 = 32;
	REG_SCROLLY2 = 32;
	REG_SCROLLY3 = 16;
	int scroll = 0;
	int col = 40;
	int animation = 0;
	for(;;)
	{
		vbl();

		//Something to test the object structs with...
		if (REG_KEYIN == 0xC8) objectsB[0].y--;
		else if (REG_KEYIN == 0xD0) objectsB[0].y++;
		else if (REG_KEYIN == 0xCB) objectsB[0].x--;
		else if (REG_KEYIN == 0xCD) objectsB[0].x++;

		REG_SCROLLX1 = scroll;
		REG_SCROLLX2 = scroll;
		REG_SCROLLX3 = scroll;
		scroll += 1;
		if (REG_TICKCOUNT % 4 == 0)
		{
			animation++;
			MISC->DmaCopy(TILESET + 0x020, (int8_t*)&questionTiles + ((animation % 9) * 128), 32, DMA_INT);
			MISC->DmaCopy(TILESET + 0x240, (int8_t*)&grassTiles + ((animation % 3) * 192), 48, DMA_INT);
		}
		if (scroll % 8 == 0)
		{
			col++;
			DrawStripe(col % MAPWIDTH, col % SCREENWIDTH);
		}
	}
}
