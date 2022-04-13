#include "../ass.h"
IBios* interface;

#define MAPWIDTH 130
#define SCREENWIDTH 64

#define F2I(x) (x >> 8)
#define I2F(x,y) ((x << 8) + y)

extern const uint16_t tilesetPal[], tilesetTiles[], hdma1[];
extern const uint16_t map16[];
extern const uint16_t farahPal[], farahTiles[];
extern const uint16_t tileanimTiles[];
extern const uint16_t bg1Map[], bg2Map[], levelMap[];

const uint16_t palAnim[] =
{
	0x7708,0x7FFF,0x1084,0x1BDF,0x027F,0x0139,0x1ADA,0x0DF3,0x050D,0x671B,0x4A34,0x2D4D,0x3FFF,0x45BF,0x20D4,0x2D3A,
	0x7708,0x7FFF,0x1084,0x4BFF,0x1F5F,0x0D9C,0x2B5E,0x1E77,0x1170,0x7E97,0x6DB0,0x54EA,0x3FFF,0x45BF,0x20D4,0x2D3A,
	0x7708,0x7FFF,0x1084,0x4BFF,0x1F5F,0x0D9C,0x2B5E,0x1E77,0x1170,0x477F,0x3298,0x15B1,0x3FFF,0x45BF,0x20D4,0x2D3A,
	0x7708,0x7FFF,0x1084,0x7FFF,0x4BFF,0x19FF,0x3BDF,0x2EFB,0x1DD3,0x671B,0x4A34,0x2D4D,0x3FFF,0x45BF,0x20D4,0x2D3A,
	0x7708,0x7FFF,0x1084,0x7FFF,0x4BFF,0x19FF,0x3BDF,0x2EFB,0x1DD3,0x7E97,0x6DB0,0x54EA,0x3FFF,0x45BF,0x20D4,0x2D3A,
	0x7708,0x7FFF,0x1084,0x7FFF,0x4BFF,0x19FF,0x3BDF,0x2EFB,0x1DD3,0x477F,0x3298,0x15B1,0x3FFF,0x45BF,0x20D4,0x2D3A,
};

void DrawTile(int x, int y, int num, uint16_t* map)
{
	x &= 31;
	y &= 31;
	int pos = ((y * 2) * 64) + (x * 2);

	map[pos +  0] = map16[(num * 4) + 0] + 256;
	map[pos +  1] = map16[(num * 4) + 1] + 256;
	map[pos + 64] = map16[(num * 4) + 2] + 256;
	map[pos + 65] = map16[(num * 4) + 3] + 256;
}

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


const uint8_t farahSprites[][6] =
{
	{  0, 1, 3, 4,13,14 }, //stand
	{  2, 1, 5, 6,15,16 }, //walk 1
	{  2, 1, 7, 8,17,18 }, //walk 2
	{  2, 1, 9,10,19,20 }, //walk 3
	{  2, 1, 7, 8,17,18 }, //walk 4 (2 again)
	{  2, 1,11,10,21,22 }, //jump
	{  2, 1,12,10,23,24 }, //fall
	{ 27,27, 0, 1,25,26 }, //duck
};

typedef struct
{
	uint32_t posX;
	uint32_t posY;
	uint8_t state;
	uint8_t frame;
	uint8_t timer;
	uint8_t flip;
	int16_t impY;
	uint8_t grounded;
} TPlayer;

TPlayer player = { 0 };

void BuildPlayer()
{
	const char offsets[][2] = {
		{  0, 0 }, {  8, 0 },
		{  0, 8 }, {  8, 8 },
		{  0,16 }, {  8,16 },
	};
	if (!player.flip)
	{
		for (int i = 0; i < 6; i++)
		{
			objectsA[i].tile = farahSprites[player.frame][i] + 1;
			objectsA[i].enabled = 1;
			objectsB[i].x = player.posX + offsets[i][0];
			objectsB[i].y = player.posY + offsets[i][1];
			objectsB[i].flipH = 0;
		}
	}
	else
	{
		for (int i = 0; i < 6; i++)
		{
			objectsA[i].tile = farahSprites[player.frame][i ^ 1] + 1;
			objectsA[i].enabled = 1;
			objectsB[i].x = player.posX + offsets[i][0];
			objectsB[i].y = player.posY + offsets[i][1];
			objectsB[i].flipH = 1;
		}
	}
}

void MovePlayer()
{
	player.posY += F2I(player.impY);
	player.impY += 16;
	if (player.impY > I2F(8,0))
		player.impY = I2F(8,0);
	if (player.impY > 0)
	{
		player.state = 3;
		player.frame = 6;
	}
	if (player.impY > 0 && player.posY >= 184)
	{
		player.posY = 184;
		player.grounded = 1;
		player.impY = 0;
		player.state = 0;
		player.frame = 0;
	}

	int dpadbuts = INP_JOYPAD1;
	if (dpadbuts & 0x10)
	{
		if (player.state < 2 && player.grounded)
		{
			player.impY = -I2F(2,-32);
			player.state = 2;
			player.frame = 5;
			player.grounded = 0;
		}
	}
	if (dpadbuts & 2)
	{
		player.flip = 0;
		player.posX++;
		if (player.state < 2)
		{
			player.state = 1;
			player.timer++;
			player.frame = 1 + ((player.timer / 8) % 4);
		}
	}
	else if (dpadbuts & 8)
	{
		player.flip = 1;
		player.posX--;
		if (player.state < 2)
		{
			player.state = 1;
			player.timer++;
			player.frame = 1 + ((player.timer / 8) % 4);
		}
	}
	else
	{
		player.timer = 0;
	}

	MAP3[0] = player.state + 256 + 0x1000;
	MAP3[2] = player.grounded + 256 + 0x1000;
}

void BarnDoorsIn()
{
	REG_WINLEFT = 160;
	REG_WINRIGHT = 160;
	REG_WINMASK = 1 | 2 | 4 | 8 | 16 | 32;
	REG_SCREENFADE = 0;
	vbl();
	for (int i = 0; i <= 160; i += 2)
	{
		REG_WINLEFT = 160 - i;
		REG_WINRIGHT = 160 + i;
		vbl();
	}
	REG_WINMASK = 0;
}

void BarnDoorsOut()
{
	REG_WINLEFT = 0;
	REG_WINRIGHT = 320;
	REG_WINMASK = 1 | 2 | 4 | 8 | 16 | 32;
	REG_SCREENFADE = 0;
	vbl();
	for (int i = 0; i <= 160; i += 2)
	{
		REG_WINLEFT = i;
		REG_WINRIGHT = 319 - i;
		vbl();
	}
	REG_SCREENFADE = 31;
	REG_WINMASK = 0;
}

int main(void)
{
	REG_SCREENMODE = SMODE_TILE;
	REG_SCREENFADE = 31;

	intoff();

	MISC->DmaClear(TILESET, 0, 0x4000, DMA_INT);
	MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);

	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	MISC->DmaCopy(TILESET + (32*256), (int8_t*)&tilesetTiles, 20480 / 4, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&tilesetPal, 128, DMA_SHORT);

	MISC->DmaCopy(TILESET + 32, (int8_t*)&farahTiles, 384, DMA_INT);
	MISC->DmaCopy(PALETTE + 256, (int8_t*)&farahPal, 16, DMA_SHORT);

	REG_MAPSET = 0x70; //just enable it, don't worry about tile offsets.
	//REG_MAPBLEND = 0x01;

	//for (int i = 0; i < SCREENWIDTH; i++)
	//	DrawStripe(i, i);
	MISC->DmaClear(MAP1, 0, 0x4000, DMA_SHORT);
	for (int i = 0; i < 20; i++)
	{
		DrawTile(i, 13, 0x41, MAP1);
		DrawTile(i, 14, 0x42, MAP1);
	}
	for (int j = 0, k = 0; j < 12; j++)
		for (int i = 0; i < 16 && k < 0xAF; i++, k++)
			DrawTile(i, j, k, MAP1);

	{
		const char buffer[] = "TILE TEST - V3";
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

//	inton();

	REG_SCROLLY1 = 0;
	REG_SCROLLY2 = 0;
	REG_SCROLLY3 = 0;
//	int scroll = 0;
//	int col = 40;
	int animation = 0;
	MISC->DmaCopy(TILESET + (32*0x1C0), (int8_t*)&tileanimTiles + ((animation % 8) * (32*4*16)), (8*4*16), DMA_INT);

	player.posX =  9 * 16;
	player.posY = 2 * 16; //11 * 16;
	player.frame = 0;
	player.impY = 2 << 8;
	BuildPlayer();

//	BarnDoorsIn();
//	BarnDoorsOut();
	REG_SCREENFADE = 0;

	for(;;)
	{
		vbl();

		//SMB1
//		REG_SCROLLX3 = 0;
//		while (REG_LINE < 32);

		//Something to test the object structs with...
//		int dpadbuts = INP_JOYPAD1;
//		if (dpadbuts & 4) objectsB[0].y++;
//		else if (dpadbuts & 1) objectsB[0].y--;
//		if (dpadbuts & 2) objectsB[0].x++;
//		else if (dpadbuts & 8) objectsB[0].x--;

		MovePlayer();
		BuildPlayer();

//		REG_SCROLLX1 = scroll;
//		REG_SCROLLX2 = scroll;
//		REG_SCROLLX3 = scroll;
//		scroll += 1;
		if (REG_TICKCOUNT % 8 == 0)
		{
			animation++;
			MISC->DmaCopy(TILESET + (32*0x1C0), (int8_t*)&tileanimTiles + ((animation % 8) * (32*4*16)), (8*4*16), DMA_INT);
			MISC->DmaCopy(PALETTE + 16, (int8_t*)&palAnim + ((animation % 6) * 32), 16, DMA_SHORT);
		}
//		if (scroll % 8 == 0)
//		{
//			col++;
//			DrawStripe(col % MAPWIDTH, col % SCREENWIDTH);
//		}

		//SMB3
//		while (REG_LINE < 400);
//		REG_SCROLLX3 = REG_SCROLLY3 = 0;
	}
}
