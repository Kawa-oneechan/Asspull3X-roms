#include "../ass.h"
#include "../ass-keys.h"
#include "actor.h"

extern const uint16_t* const playerTiles[];
extern const uint16_t* const playerPal[];

typedef struct
{
	int16_t type;
	int16_t group;
	int16_t x, y;
	int16_t oldX, oldY;
	uint8_t pal;
	uint8_t obj;
	int8_t state;
	void (*draw)(), (*think)();
	int16_t flameTick, swerve, intro, shotTimer;
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

	OBJECTS_A[(int)p->obj] = OBJECTA_BUILD(256 + (frame * 8), 0, 1, p->pal);
	OBJECTS_B[(int)p->obj] = OBJECTB_BUILD(p->x, p->y, 1, 0, 0, 0, 1, 0);
	OBJECTS_A[(int)p->obj+1] = OBJECTA_BUILD(256 + 40 + ((p->flameTick >> 2) % 2), 1, 1, 0);
	OBJECTS_B[(int)p->obj+1] = OBJECTB_BUILD(p->x - 8, p->y + 4, 0, 0, 0, 0, 0, 0);
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
		int dpadbuts = INP_JOYPAD1;

		if (dpadbuts & BUTTON_DOWN) p->y++;
		else if (dpadbuts & BUTTON_UP) p->y--;
		if (dpadbuts & BUTTON_RIGHT) p->x++;
		else if (dpadbuts & BUTTON_LEFT) p->x--;

		if (dpadbuts & BUTTON_A)
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

	p->pal = 0;
	p->x -= 64; //to fly in
	p->obj = GetNextObjectIdx(0, 4);
	bitSet(objectsUsed, p->obj);
	bitSet(objectsUsed, p->obj + 1);

	MISC->DmaCopy(TILESET + 0x2000, playerTiles[0], 512, DMA_INT);
	MISC->DmaCopy(PALETTE + 256, playerPal[0], 16, DMA_SHORT);
}

void DrawPlayerBullet(int id)
{
	tEntity* p = &entities[id];

	OBJECTS_A[(int)p->obj] = OBJECTA_BUILD(256 + 42, 0, 1, p->pal);
	OBJECTS_B[(int)p->obj] = OBJECTB_BUILD(p->x, p->y, 1, 0, 0, 0, 0, 0);
}

void ThinkPlayerBullet(int id)
{
	tEntity* p = &entities[id];
	p->x += 4;
	if (p->x > 320)
	{
		p->type = 0;
		bitClear(objectsUsed, p->obj);
	}
}

void InitPlayerBullet(int id)
{
	tEntity* p = &entities[id];
	p->pal = 0;
	p->obj = GetNextObjectIdx(4, 24);
	bitSet(objectsUsed, p->obj);
}
