#include "rpg.h"

static const char xdisp[] = { 0, -1, 0, 1 };
static const char ydisp[] = { 1, 0, -1, 0 };

int cameraX, cameraY;
int cameraTX, cameraTY, lastCameraTX, lastCameraTY;

Map* map;
MapEntity entities[MAXENTITIES];
MapEntity* playerEntity;
int oidCt;

void drawTile(int x, int y, int tileNum)
{
	x &= 31;
	y &= 31;
	int16_t* tile = map->tileset->metatiles + (tileNum * 9);
	int pos = ((y * 2) * 64) + (x * 2);

	MAP1[pos +  0] = tile[1];
	MAP1[pos +  1] = tile[2];
	MAP1[pos + 64] = tile[3];
	MAP1[pos + 65] = tile[4];

	if (tile[0] & 0x2000)
	{
		MAP3[pos +  0] = tile[5];
		MAP3[pos +  1] = tile[6];
		MAP3[pos + 64] = tile[7];
		MAP3[pos + 65] = tile[8];
	}
	else
	{
		MAP2[pos +  0] = tile[5];
		MAP2[pos +  1] = tile[6];
		MAP2[pos + 64] = tile[7];
		MAP2[pos + 65] = tile[8];
	}
}

void aimCamera(int x, int y)
{
	int maxExtX = (map->width - 20) * 16;
	int maxExtY = (map->height - 15) * 16;

	cameraX = x - (9 * 16);
	cameraY = y - (6 * 16);
	if (cameraX < 0) cameraX = 0;
	if (cameraY < 0) cameraY = 0;
	if (cameraX > maxExtX) cameraX = maxExtX;
	if (cameraY > maxExtY) cameraY = maxExtY;

	if (map->width <= 20)
		cameraX = 0;
	if (map->height <= 15)
		cameraY = 0;

	cameraTX = cameraX / 16;
	cameraTY = cameraY / 16;


	if (cameraTX != lastCameraTX)
	{
		int x = cameraTX + (cameraTX > lastCameraTX ? 20 : 0);
		for (int y = cameraTY; y < cameraTY + 16; y++)
			drawTile(x, y, map->map[(y * map->width) + x]);
		lastCameraTX = cameraTX;
	}
	if (cameraTY != lastCameraTY)
	{
		int y = cameraTY + (cameraTY > lastCameraTY ? 15 : 0);
		for (int x = cameraTX; x < cameraTX + 21; x++)
			drawTile(x, y, map->map[(y * map->width) + x]);
		lastCameraTY = cameraTY;
	}

	REG_SCROLLX1 = cameraX;
	REG_SCROLLX2 = cameraX;
	REG_SCROLLX3 = cameraX;
	REG_SCROLLY1 = cameraY;
	REG_SCROLLY2 = cameraY;
	REG_SCROLLY3 = cameraY;
}

void drawMap()
{
	for (int y = cameraTY - 1; y < cameraTY + 16; y++)
		for (int x = cameraTX - 1; x < cameraTX + 21; x++)
			drawTile(x, y, map->map[(y * map->width) + x]);
}

void drawEntity(MapEntity *entity)
{
	if (entity->oid == 255) return;
	int oid = 65 - entity->oid;

	const char stepFrames[] = { 8,8,8,8,8,8,8,8,0,0,0,0,0,0,0,0,16,16,16,16,16,16,16,16,0,0,0,0,0,0,0,0 };

	int tile = 0;
	int flip = 0;
	int x = entity->x * 16;
	int y = (entity->y * 16) - 20;
	int xd = 0, yd = 0;

	switch (entity->state)
	{
		case stateIdle:
		case stateStep:
			switch (entity->facing)
			{
				case faceDown:
					yd += entity->counter[0];
					break;
				case faceLeft:
					tile = 24;
					xd -= entity->counter[0];
					break;
				case faceUp:
					tile = 48;
					yd -= entity->counter[0];
					break;
				case faceRight:
					tile = 24;
					flip = 1;
					xd += entity->counter[0];
					break;
			}
			if (entity->state == stateIdle)
				break;
			tile += stepFrames[entity->counter[0] + (entity->counter[1] * 16)];
			x += xd;
			y += yd;
			break;
		case stateTsk:
			tile = 72;
			if (entity->counter[0] % 32 >= 16)
				tile += 8;
			break;
		case stateWave:
			tile = 88;
			break;
		case stateLaugh:
			tile = 96;
			if (entity->counter[0] % 32 >= 16)
				tile += 8;
			break;
	}

	if (entity == playerEntity)
		aimCamera(x, y);

	MISC->DmaClear(TILESET + (oid * 256), 0, 64, DMA_INT);
	MISC->DmaCopy(TILESET + (oid * 256) + 64, entity->tileset + (tile * 24), 48, DMA_INT);
	OBJECTS_A[entity->oid] = OBJECTA_BUILD(oid * 8, 0, 1, entity->palette);
	OBJECTS_B[entity->oid] = OBJECTB_BUILD(x - cameraX, y - cameraY, 0, 1, flip, 0, 1, 2);
}

void updateEntity(MapEntity *entity)
{
//	if (entity->oid == 255) return;

	//adapted from OpenPoke
	bool offscreen = false;
	int bsv = entity->x - playerEntity->x;
	if(bsv < -10) offscreen = true;
	if(bsv > 10) offscreen = true;
	bsv = entity->y - playerEntity->y;
	if(bsv < -9) offscreen = true;
	if(bsv > 9) offscreen = true;
	if (offscreen)
		entity->oid = 255;
	else
		entity->oid = 64 - oidCt++;

	if (entity->motor != 0)
	{
		void (*motor)(MapEntity*) = entity->motor;
		motor(entity);
	}

	switch (entity->state)
	{
		case stateIdle:
			entity->counter[0] = 0;
			break;
		case stateStep:
			entity->counter[0]++;
			if (entity->counter[0] == 16)
			{
				entity->counter[1] ^= 1;
				entity->state = stateIdle;
				entity->x += xdisp[entity->facing];
				entity->y += ydisp[entity->facing];
			}
			break;
		case stateTsk:
		case stateWave:
		case stateLaugh:
			entity->counter[0]++;
//			if (entity->counter[0] == 32)
//				entity->state = stateIdle;
			break;
	}
}

int entityIsBlocked(MapEntity *entity)
{
	int x = entity->x + xdisp[entity->facing];
	int y = entity->y + ydisp[entity->facing];
	int here = map->map[(y * map->width) + x];
	return map->tileset->metatiles[here * 9] & 0x4000;
}

void entityWalk(MapEntity *entity, int facing)
{
	if (entity->state == 1) return;
	entity->facing = facing;
	if (entityIsBlocked(entity))
		return;
	int tx = entity->x + xdisp[entity->facing];
	int ty = entity->y + ydisp[entity->facing];
	entity->counter[0] = 0;
	for (int i = 0; i < map->entities[0]; i++)
	{
		if (&entities[i] == entity)
			continue;
		if (entities[i].x == tx && entities[i].y == ty)
			return;
	}
	entity->state = stateStep;
}

void entityPlayerMotor(MapEntity *entity)
{
	switch (lastInput)
	{
		case KEYSCAN_LEFT: entityWalk(entity, 1); break;
		case KEYSCAN_RIGHT: entityWalk(entity, 3); break;
		case KEYSCAN_UP: entityWalk(entity, 2); break;
		case KEYSCAN_DOWN: entityWalk(entity, 0); break;
		case KEYSCAN_A: entity->state = stateTsk; entity->counter[0] = 0; break;
		case KEYSCAN_S: entity->state = stateWave; entity->counter[0] = 0; break;
		case KEYSCAN_D: entity->state = stateLaugh; entity->counter[0] = 0; break;
		case KEY_ACTION:
		{
			int tx = entity->x + xdisp[entity->facing];
			int ty = entity->y + ydisp[entity->facing];
			for (int i = 0; i < map->entities[0]; i++)
			{
				if (&entities[i] == entity)
					continue;
				if (entities[i].x == tx && entities[i].y == ty)
				{
					if (entities[i].script != 0)
						runScript(entities[i].script, i);
					return;
				}
			}
		}
	}
}

//I don't understand half the shit Cearn did here.
void id_sort_shell(int keys[], uint8_t ids[], int count)
{
	int ii, inc, jj, id0;
	int key0;

	for(inc = 1; inc <= count; inc++)
		inc *= 3;
	do
	{
		inc = (inc * 0x5556) >> 16;
		for (ii = inc; ii < count; ii++)
		{
			id0 = ids[ii];
			key0= keys[id0];
			for (jj = ii; jj >= inc && keys[ids[jj - inc]] > key0; jj -= inc)
				ids[jj] = ids[jj - inc];
			ids[jj] = id0;
		}
	} while(inc > 1);
}

void updateAndDraw()
{
	int keys[MAXENTITIES];
	uint8_t ids[MAXENTITIES];
	int count = 0;

	for(int i = 0; i < map->entities[0]; i++)
	{
		ids[i] = i;
		keys[i] = entities[i].oid;
//		if(Actors[i].sprite)
//		{
			keys[i] = -256 * entities[i].y + i;
			count++;
//		}
	}
	id_sort_shell(keys, ids, count);
	oidCt = 0;
	for (int i = 0; i < MAXENTITIES; i++)
		OBJECTS_A[i] = 0;

	for (int i = 0; i < count; i++)
	{
		//entities[ids[i]].oid = count - i;
		updateEntity((MapEntity*)&entities[ids[i]]);
		drawEntity((MapEntity*)&entities[ids[i]]);
	}
}

void loadMap(Map* newMap)
{
	map = newMap;
	//MISC->DmaClear(TILESET + 0x4000, 0x04040404, 0x2000, DMA_INT);
	MISC->DmaCopy(TILESET + 0x4000, map->tileset->graphics, 0x2000, DMA_INT);
	MISC->DmaCopy(PALETTE, map->tileset->palette, 72, DMA_INT);
	uint8_t* data = map->entities + 1;
	for (int i = 0; i < map->entities[0]; i++)
	{
		MapEntity* me = &entities[i];
		me->x = data[1];
		me->y = data[2];
		me->facing = data[3];
		me->state = 0;
		me->counter[0] = 0;
		me->counter[1] = 0;
		me->counter[2] = 0;
		me->counter[3] = 0;
		me->tileset = (int8_t*)sprites[data[0]];
		me->palette = spritePals[data[0]];
		me->script = (uint8_t*)*(int32_t*)(&data[6]);
		switch (data[4])
		{
			case 255:
				me->motor = entityPlayerMotor;
				aimCamera(me->x * 16, me->y * 16);
				playerEntity = me;
				break;
			default: me->motor = 0; break;
		}
		data += 10;
	}
}
