#include "../ass.h"
IBios* interface;

#define MAPWIDTH 130
#define SCREENWIDTH 64

extern const uint16_t tilesetPal[], tilesetTiles[], hdma1[];
extern const uint16_t farahPal[], farahTiles[];
extern const uint16_t tileanimTiles[];
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
		if (*s1 > 256)
			*d1 = *s1;
		else
			*d1 = 0;
		if (*s2 > 256)
			*d2 = *s2;
		else
			*d2 = 0;
		if (i > 6)
		{
			if (*s3 > 256)
				*d3 = *s3;
			else
				*d3 = 0;
		}
		s1 += MAPWIDTH;
		s2 += MAPWIDTH;
		s3 += MAPWIDTH;
		d1 += SCREENWIDTH;
		d2 += SCREENWIDTH;
		d3 += SCREENWIDTH;
	}
}

typedef struct
{
	uint16_t palette:4;
	uint16_t enabled:1;
	uint16_t blend:2;
	uint16_t tile:9;
} TObjectA;
#define objectsA ((TObjectA*)OBJECTS_A)
typedef struct
{
	union
	{
		struct
		{
			uint32_t priority:3;
			uint32_t large:1;

			uint32_t flipV:1;
			uint32_t flipH:1;
			uint32_t tall:1;
			uint32_t wide:1;

			int32_t :2;
			int32_t y:10;
			int32_t :1;
			int32_t x:11;
		};
		uint32_t raw;
	};
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

	MISC->DmaCopy(TILESET + (32*256), (int8_t*)&tilesetTiles, 20480 / 4, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&tilesetPal, 128, DMA_SHORT);

	MISC->DmaCopy(TILESET + 32, (int8_t*)&farahTiles, 64, DMA_INT);
	MISC->DmaCopy(PALETTE + 256, (int8_t*)&farahPal, 16, DMA_SHORT);

	objectsA[0].tile = 1;
	objectsA[0].enabled = 1;
	objectsB[0].x = 152;
	objectsB[0].y = 176;
	objectsB[0].tall = 1;
	objectsB[0].large = 1;

	REG_MAPSET = 0x70; //just enable it, don't worry about tile offsets.
	//REG_MAPBLEND = 0x01;

	for (int i = 0; i < SCREENWIDTH; i++)
		DrawStripe(i, i);

	{
		const char buffer[] = "TILE TEST - V2";
		char *c = (char*)buffer;
		int pos = 65;
		while (*c)
		{
			if (*c >= '0' && *c <= '9')
				MAP3[pos] = *c - '0' + 256;
			else if (*c >= 'A' && *c <= 'Z')
				MAP3[pos] = *c - 'A' + 266;
			else if (*c == '-')
				MAP3[pos] = 296;
			c++;
			pos++;
		}
	}

	inton();

	REG_SCROLLY1 = 0;
	REG_SCROLLY2 = 0;
	REG_SCROLLY3 = 0;
	int scroll = 0;
	int col = 40;
	int animation = 0;
	for(;;)
	{
		vbl();

		//SMB1
		REG_SCROLLX3 = 0;
		while (REG_LINE < 32);

		//Something to test the object structs with...
		REG_JOYPAD = 1;
		int dpadbuts = REG_JOYPAD;
		if (dpadbuts & 4) objectsB[0].y++;
		else if (dpadbuts & 1) objectsB[0].y--;
		if (dpadbuts & 2) objectsB[0].x++;
		else if (dpadbuts & 8) objectsB[0].x--;

		REG_SCROLLX1 = scroll;
		REG_SCROLLX2 = scroll;
		REG_SCROLLX3 = scroll;
		scroll += 1;
		if (REG_TICKCOUNT % 4 == 0)
		{
			animation++;
			MISC->DmaCopy(TILESET + (32*0x1C0), (int8_t*)&tileanimTiles + ((animation % 8) * (32*4*16)), (8*4*16), DMA_INT);
		}
		if (scroll % 8 == 0)
		{
			col++;
			DrawStripe(col % MAPWIDTH, col % SCREENWIDTH);
		}

		//SMB3
		//while (REG_LINE < 400);
		//REG_SCROLLX3 = REG_SCROLLY3 = 0;
	}
}
