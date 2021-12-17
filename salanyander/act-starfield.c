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
		bitClear(objectsUsed, p->obj);
	}
}

void DrawStar(int id)
{
	tEntity* p = &entities[id];
	OBJECTS_A[(int)p->obj] = OBJECTA_BUILD(p->extra[0], 0, 1, p->pal);
	OBJECTS_B[(int)p->obj] = OBJECTB_BUILD(p->x, p->y, 0, 0, 0, 0, 0, 1);
}

void InitStar(int id)
{
	tEntity* p = &entities[id];
	p->extra[0] = 73 + (rand() % 3);
	p->obj = GetNextObjectIdx(4, 128);
	bitSet(objectsUsed, p->obj);
}
