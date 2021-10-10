#include "../ass.h"
#include "actor.h"

extern int scroll;

void ThinkStarfield(int id)
{
	tEntity* p = &entities[id];
	p->extra[0]++;
	if (p->extra[0] > 16)
	{
		p->extra[0] = 0;
		Spawn(4, 320, rand() % 240);
	}
	if (p->extra[1] && scroll >= p->extra[1])
	{
		p->type = 0;
	}
}

void ThinkStar(int id)
{
	tEntity* p = &entities[id];
	p->x -= 2;
	if (p->x < -16)
	{
		p->type = 0;
		bitClear(spritesUsed, p->spr);
	}
}

void DrawStar(int id)
{
	tEntity* p = &entities[id];
	SPRITES_A[(int)p->spr] = SPRITEA_BUILD(p->extra[0], 0, 1, p->pal);
	SPRITES_B[(int)p->spr] = SPRITEB_BUILD(p->x, p->y, 0, 0, 0, 0, 0, 1);
}

void InitStar(int id)
{
	tEntity* p = &entities[id];
	p->extra[0] = 73 + (rand() % 3);
	p->spr = GetNextSpriteIn(4, 128);
	bitSet(spritesUsed, p->spr);
}
