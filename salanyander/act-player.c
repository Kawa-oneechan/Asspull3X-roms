#include "../ass.h"
#include "actor.h"

extern const unsigned short* const playerTiles[];
extern const unsigned short* const playerPal[];

typedef struct tPlayer
{
	short type;
	short group;
	short x, y;
	short oldX, oldY;
	char pal;
	char spr;
	char state;
	void (*draw)(), (*think)();
	short flameTick, swerve, intro, shotTimer;
} tPlayer;

void DrawPlayer(int id)
{
	tPlayer* p = (tPlayer*)&entities[id];

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

	SPRITES_A[(int)p->spr] = SPRITEA_BUILD(256 + (frame * 8), 0, 1, p->pal);
	SPRITES_B[(int)p->spr] = SPRITEB_BUILD(p->x, p->y, 1, 0, 0, 0, 1, 0);
	SPRITES_A[(int)p->spr+1] = SPRITEA_BUILD(256 + 40 + ((p->flameTick >> 2) % 2), 1, 1, 8);
	SPRITES_B[(int)p->spr+1] = SPRITEB_BUILD(p->x - 8, p->y + 4, 0, 0, 0, 0, 0, 0);
}

void ThinkPlayer(int id)
{
	tPlayer* p = (tPlayer*)&entities[id];

	if (p->state == 0) //flying in
	{
		if (p->intro < 64)
		{
			p->intro++;
			p->x++;
		}
		else
		{
			p->state = 1; //player control
		}
	}
	else
	{
		if (REG_KEYIN == 0xD0) p->y++;
		else if (REG_KEYIN == 0xC8) p->y--;
		else if (REG_KEYIN == 0xCD) p->x++;
		else if (REG_KEYIN == 0xCB) p->x--;

		if (REG_KEYIN == 0x2E)
		{
			if (p->shotTimer == 0)
			{
				Spawn(2, p->x + 12, p->y + 4);
				p->shotTimer = 5;
			}
		}
		else if (p->shotTimer > 0)
		{
			p->shotTimer--;
		}
	}

	p->flameTick++;
}

void InitPlayer(int id)
{
	tPlayer* p = (tPlayer*)&entities[id];

	p->pal = 8;
	p->x -= 64; //to fly in
	p->spr = GetNextSpriteIn(0, 4);
	bitSet(spritesUsed, p->spr);
	bitSet(spritesUsed, p->spr + 1);

	MISC->DmaCopy(TILESET + 0x2000, playerTiles[0], 512, DMA_INT);
	MISC->DmaCopy(PALETTE + 128, playerPal[0], 16, DMA_SHORT);
}

void DrawPlayerBullet(int id)
{
	tEntity* p = &entities[id];

	SPRITES_A[(int)p->spr] = SPRITEA_BUILD(256 + 42, 0, 1, p->pal);
	SPRITES_B[(int)p->spr] = SPRITEB_BUILD(p->x, p->y, 1, 0, 0, 0, 0, 0);
}

void ThinkPlayerBullet(int id)
{
	tEntity* p = &entities[id];
	p->x += 4;
	if (p->x > 320)
	{
		p->type = 0;
		bitClear(spritesUsed, p->spr);
	}
}

void InitPlayerBullet(int id)
{
	tEntity* p = &entities[id];
	p->pal = 8;
	p->spr = GetNextSpriteIn(4, 24);
	bitSet(spritesUsed, p->spr);
}
