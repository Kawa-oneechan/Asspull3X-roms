#include "../ass.h"
#include "actor.h"

tEntity entities[MAXENTITIES];

int objectsUsed[256/32];
uint32_t scroll = 0;

const void* const entityFuncs[] =
{
	0, 0, 0, 0,
	ThinkPlayer, DrawPlayer, InitPlayer, 0,
	ThinkPlayerBullet, DrawPlayerBullet, InitPlayerBullet, 0,
	ThinkStarfield, 0, 0, 0,
	ThinkStar, DrawStar, InitStar, 0,
};

void Think()
{
	for (int i = 0; i < MAXENTITIES; i++)
	{
		if (entities[i].type == 0)
			continue;
		if (entities[i].think != 0)
			entities[i].think(i);
	}
}

void Draw()
{
	for (int i = 0; i < MAXENTITIES; i++)
	{
		if (entities[i].type == 0)
			continue;
		if (entities[i].draw != 0)
			entities[i].draw(i);
	}
}

int Spawn(int16_t type, int16_t x, int16_t y)
{
	for (int i = 0; i < MAXENTITIES; i++)
	{
		if (entities[i].type != 0)
			continue;
		tEntity* e = &entities[i];
		e->type = type;
		e->x = e->oldX = x;
		e->y = e->oldY = y;
		e->think = entityFuncs[(type * 4) + 0];
		e->draw = entityFuncs[(type * 4) + 1];
		void (*init)(int) = (void*)entityFuncs[(type * 4) + 2];
		if (init != 0)
			init(i);
		return i;
	}
	return -1;
}

int GetNextObjectIdx(int min, int max)
{
	for (int i = min; i < max; i++)
	{
		if (!bitTest(objectsUsed, i))
			return i;
	}
	return -1;
}

int GetNextObject()
{
	return GetNextObjectIdx(0, 256);
}
