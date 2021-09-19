#include "../ass.h"
IBios* interface;

extern const uint16_t fontTiles[], fontPal[];
extern const uint16_t playerTiles[], playerPal[];

#define SPRITEA_BUILD(t,b,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((b) &  3) <<  9) |		\
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

typedef struct tEntity
{
	short type;
	short group;
	short x, y;
	short oldX, oldY;
	char pal;
	char spr;
	void (*draw)(), (*think)();
	short extra[16];
} tEntity;

tEntity entities[64];

//===== PLAYER =====
typedef struct tPlayer
{
	short type;
	short group;
	short x, y;
	short oldX, oldY;
	char pal;
	char spr;
	void (*draw)(), (*think)();
	short flameTick, swerve;
} tPlayer;

void DrawPlayer(int id)
{
	tPlayer* p = (tPlayer*)&entities[id];

	const char flameOffsets[] = { 7, 6, 4, 4, 4 };
	int frame = 0;

	if (p->y < p->oldY)
		p->swerve--;
	else if (p->y > p->oldY)
		p->swerve++;
	else
	{
		if (p->swerve > 0) p->swerve--;
		else if (p->swerve < 0) p->swerve++;
	}
	p->oldY = p->y;
	if (p->swerve < 0) frame = 1;
	if (p->swerve < -16) { frame = 2; p->swerve = -16; }
	if (p->swerve > 0) frame = 3;
	if (p->swerve > 16) { frame = 4; p->swerve = 16; }

	SPRITES_A[(int)p->spr] = SPRITEA_BUILD(128 + (frame * 8), 0, 1, p->pal);
	SPRITES_B[(int)p->spr] = SPRITEB_BUILD(p->x, p->y, 1, 0, 0, 0, 1, 0);
	SPRITES_A[(int)p->spr+1] = SPRITEA_BUILD(128 + 40 + ((p->flameTick >> 2) % 2), 1, 1, 2);
	SPRITES_B[(int)p->spr+1] = SPRITEB_BUILD(p->x - 8, p->y + flameOffsets[frame], 0, 0, 0, 0, 0, 0);
}

void ThinkPlayer(int id)
{
	tPlayer* p = (tPlayer*)&entities[id];

	if (REG_KEYIN == 0xD0) p->y++;
	else if (REG_KEYIN == 0xC8) p->y--;
	else if (REG_KEYIN == 0xCD) p->x++;
	else if (REG_KEYIN == 0xCB) p->x--;

	p->flameTick++;
}

int main(void)
{
	REG_SCREENMODE = SMODE_TILE;
	REG_SCREENFADE = 0;

	intoff();

	MISC->DmaClear(entities, 0, sizeof(tEntity) * 64, DMA_BYTE);

	MISC->DmaCopy(TILESET, (int8_t*)&fontTiles, 512, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&fontPal, 16, DMA_SHORT);

	MISC->DmaCopy(TILESET + 0x1000, (int8_t*)&playerTiles, 512, DMA_INT);
	MISC->DmaCopy(PALETTE + 32, (int8_t*)&playerPal, 32, DMA_SHORT);

	entities[0].type = 0x42;
	entities[0].x = 32;
	entities[0].y = entities[0].oldY = 128;
	entities[0].spr = 0;
	entities[0].pal = 2;
	entities[0].draw = DrawPlayer;
	entities[0].think = ThinkPlayer;

	entities[1].type = 0x42;
	entities[1].x = 24;
	entities[1].y = entities[1].oldY = 64;
	entities[1].spr = 4;
	entities[1].pal = 3;
	entities[1].draw = DrawPlayer;
	entities[1].think = ThinkPlayer;
	entities[1].extra[0] = 4;

	for(;;)
	{
		for (int i = 0; i < 64; i++)
		{
			if (entities[i].type == 0)
				continue;
			entities[i].think(i);
			entities[i].draw(i);
		}

		vbl();
	}
}
