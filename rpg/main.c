#include "../ass.h"
IBios* interface;

#define WIDTH 32
#define HEIGHT 32

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0
#define KEY_ACTION 45

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
	(((vp) & 0x3FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)

extern char *strcpy(char *dest, const char *src);
extern int strlen(const char *str);

void drawWindow(int l, int t, int w, int h);
void eraseWindow(int l, int t, int w, int h);
void drawString(int x, int y, const char* string);
void updateAndDraw();

#define MAXENTITIES 16

extern const TImageFile titlePic;
extern const uint16_t spritepalPal[], fontTiles[], fontPal[], uiBackground[];
extern const uint8_t testMap[], spritePals[];
extern const uint32_t sprites[], portraits[];

enum facing
{
	faceDown, faceLeft, faceUp, faceRight
};
enum state
{
	stateIdle, stateStep, stateTsk, stateWave, stateLaugh, stateSad
};

typedef struct
{
	uint8_t* graphics;
	int16_t* palette;
	int16_t* metatiles;
} Tileset;

typedef struct
{
	uint8_t width, height;
	uint8_t* map;
	Tileset* tileset;
	uint8_t* entities;
} Map;

typedef struct
{
	uint8_t x, y;
	uint8_t oid;
	uint8_t facing;
	uint8_t state;
	uint8_t counter[4];
	uint8_t palette;
	int8_t* tileset;
	uint8_t* script;
	void* motor;
} MapEntity;

Map* map;
MapEntity entities[MAXENTITIES];
int lastInput;
int cameraX, cameraY;
int cameraTX, cameraTY, lastCameraTX, lastCameraTY;

int dialogueBoxIsOpen, dialoguePortrait;

char playerName[16] = "Farrah";
uint32_t scriptVariables[256] =
{
	0, 0, (uint32_t)&playerName
};

const char xdisp[] = { 0, -1, 0, 1 };
const char ydisp[] = { 1, 0, -1, 0 };

const uint8_t cursorTiles[] =
{
	0x00,0x00,0x30,0x33,0x00,0x00,0x23,0x22,0x00,0x30,0x26,0x62,0x30,0x30,0x22,0x22,
	0x63,0x63,0x22,0x22,0x23,0x24,0x22,0x22,0x23,0x24,0x22,0x22,0x63,0x64,0x22,0x22,
	0x03,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x44,0x33,0x33,0x00,0x22,0x22,0x22,0x03,
	0x22,0x22,0x22,0x32,0x22,0x66,0x66,0x03,0x62,0x36,0x33,0x00,0x62,0x34,0x00,0x00,
	0x63,0x64,0x26,0x22,0x63,0x44,0x66,0x66,0x43,0x43,0x64,0x66,0x30,0x30,0x44,0x44,
	0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x66,0x34,0x00,0x00,0x46,0x03,0x00,0x00,0x44,0x03,0x00,0x00,0x34,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x10,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x00,0x11,0x11,0x11,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x11,0x11,0x11,0x00,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x01,0x00,0x00,0x00,0x00,
};

void getInput()
{
	lastInput = INP_KEYIN;
	if (INP_JOYPAD1 & 1) lastInput = KEY_UP;
	else if (INP_JOYPAD1 & 2 ) lastInput = KEY_RIGHT;
	else if (INP_JOYPAD1 & 4 ) lastInput = KEY_DOWN;
	else if (INP_JOYPAD1 & 8 ) lastInput = KEY_LEFT;
	else if (INP_JOYPAD1 & 0x20) lastInput = KEY_ACTION;
}

void drawTile(int x, int y, int tileNum)
{
	x &= 31;
	y &= 31;
	int16_t* tile = map->tileset->metatiles + (tileNum * 9);
	int pos = ((y * 2) * 64) + (x * 2);

	MAP1[pos +  0] = tile[1] + 256;
	MAP1[pos +  1] = tile[2] + 256;
	MAP1[pos + 64] = tile[3] + 256;
	MAP1[pos + 65] = tile[4] + 256;

	if (tile[0] & 0x2000)
	{
		MAP3[pos +  0] = tile[5] + 256;
		MAP3[pos +  1] = tile[6] + 256;
		MAP3[pos + 64] = tile[7] + 256;
		MAP3[pos + 65] = tile[8] + 256;
	}
	else
	{
		MAP2[pos +  0] = tile[5] + 256;
		MAP2[pos +  1] = tile[6] + 256;
		MAP2[pos + 64] = tile[7] + 256;
		MAP2[pos + 65] = tile[8] + 256;
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

	if (entity == (MapEntity*)&entities[0])
		aimCamera(x, y);

	MISC->DmaClear(TILESET + (entity->oid * 256), 0, 64, DMA_INT);
	MISC->DmaCopy(TILESET + (entity->oid * 256) + 64, entity->tileset + (tile * 24), 48, DMA_INT);
	OBJECTS_A[entity->oid] = OBJECTA_BUILD(entity->oid * 8, 0, 1, entity->palette);
	OBJECTS_B[entity->oid] = OBJECTB_BUILD(x - cameraX, y - cameraY, 0, 1, flip, 0, 1, 2);
}

void updateEntity(MapEntity *entity)
{
	if (entity->oid == 255) return;

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

void waitForActionKey()
{
	while (lastInput != KEY_ACTION) { vbl(); getInput(); }
	lastInput = 0;
}

void saySomething(char *what, int flags)
{
	char *c = what;

	if (!dialogueBoxIsOpen)
	{
		for (int i = 0; i < 3; i++)
		{
			drawWindow(2, 4 - i, 36, 2 + (i * 2));
			vbl();
			vbl();
		}
	}
	else
		drawWindow(2, 2, 36, 6);
	dialogueBoxIsOpen = 1;

	if (dialoguePortrait)
	{
		MISC->DmaCopy(TILESET + 0x1800, (int8_t*)portraits[dialoguePortrait - 1], 0x80, DMA_INT);
		MISC->DmaCopy(PALETTE + 256 + 256 - 32, (int16_t*)portraits[dialoguePortrait - 1 + 8], 16, DMA_SHORT);
		OBJECTS_A[253] = OBJECTA_BUILD(192, 0, 1, 14);
		OBJECTS_B[253] = OBJECTB_BUILD(3 * 8, 3 * 8, 1, 1, 0, 0, 1, 0);
	}
	else
		OBJECTS_A[253] = OBJECTA_BUILD(5, 0, 0, 14);

	if (*c != 0)
	{
		int x = dialoguePortrait ? 8 : 3;
		int sx = x, sy = 3;
		while (*c != 0)
		{
			vbl();
			vbl();
			if (*c == '\b')
			{
				waitForActionKey();
				c++;
				continue;
			}
			if (*c == '\n')
			{
				if (sy == 5)
				{
					//Already have a full box. Scroll it.
					for (int i = 0; i < 2; i++)
					{
						MISC->DmaCopy(&MAP4[3 * 64], &MAP4[4 * 64], 360, DMA_BYTE);
						MISC->DmaClear(&MAP4[6 * 64] + 3, 0xF301, 34, DMA_SHORT);
						vbl(); vbl(); vbl(); vbl();
					}
					sy = 3;
				}
				sy += 2;
				sx = x;
				c++;
				continue;
			}
			int t = (*c - ' ' + 16) * 2;
			t |= 0xF300;
			MAP4[(sy * 64) + sx +  0] = t;
			MAP4[(sy * 64) + sx + 64] = t + 1;
			sx++;
			c++;
		}
		waitForActionKey();
	}

	if (flags & 1)
	{
		while (INP_KEYIN != 0) vbl();
		return;
	}

	dialogueBoxIsOpen = 0;
	OBJECTS_A[253] = OBJECTA_BUILD(5, 0, 0, 14);
	for (int i = 2; i >= 0; --i)
	{
		MISC->DmaClear(MAP4, 0, WIDTH * 8, DMA_INT);
		drawWindow(2, 4 - i, 36, 2 + (i * 2));
		vbl();
		vbl();
	}
	MISC->DmaClear(MAP4, 0, WIDTH * 8, DMA_INT);
	while (INP_KEYIN != 0) vbl();
}

int doMenu(int left, int top, char* options, int num)
{
	char *b = options;
	int len = 0;
	for (int i = 0; i < num; i++)
	{
		int nl = strlen(b);
		if (nl > len)
			len = nl;
		b += nl + 1;
	}
	drawWindow(left, top, len + 5, (num * 2) + 2);
	b = options;
	for (int i = 0; i < num; i++)
	{
		drawString(left + 3, top + 1 + (i * 2), b);
		b += strlen(b) + 1;
	}
	MISC->DmaCopy(TILESET + 32, cursorTiles, 48, DMA_INT);

	OBJECTS_A[255] = OBJECTA_BUILD(1, 0, 1, 15);
	OBJECTS_A[254] = OBJECTA_BUILD(5, 1, 1, 15);

	int choice = 0;
	lastInput = 0;
	while (lastInput != KEY_ACTION)
	{
		OBJECTS_B[255] = OBJECTB_BUILD(((left + 1) * 8) - 4, ((top + 1 + (choice * 2)) * 8) + ((REG_TICKCOUNT / 32) % 2), 0, 0, 0, 0, 1, 0);
		OBJECTS_B[254] = OBJECTB_BUILD(((left + 1) * 8) - 4, (top + 2 + (choice * 2)) * 8, 0, 0, 0, 0, 1, 0);
		vbl();
		getInput();
		switch (lastInput)
		{
			case KEY_UP:
				if (choice == 0) choice = num;
				choice--;
				while (lastInput) { vbl(); getInput(); }
				break;
			case KEY_DOWN:
				choice++;
				if (choice == num) choice = 0;
				while (lastInput) { vbl(); getInput(); }
				break;
		}
	}
	lastInput = 0;
	OBJECTS_A[255] = 0;
	OBJECTS_A[254] = 0;
	eraseWindow(left, top, len + 5, (num * 2) + 2);
	return choice;
}

#define MAXSTACK 250
#define MAXPRINTBUFFER 250

void runScript(uint8_t* code, int entityID)
{
	uint8_t* pc = code;
	uint32_t acc = 0;
	uint32_t stack[MAXSTACK] = { 0 };
	char printBuffer[MAXPRINTBUFFER] = { 0 };
	int stackSize = 0;
	int argc = 0;
	uint8_t cmd;
	while (*pc != 0xFF)
	{
		cmd = *pc++;

		switch(cmd)
		{
			case 0x00: break; //nop
			case 0x01: //add
			{
				argc = stack[--stackSize];
				acc = 0;
				while (argc--)
				{
					acc += stack[--stackSize];
				}
				stack[stackSize++] = acc;
				break;
			}
			case 0x02: //sub
			{
				argc = stack[--stackSize] - 1;
				acc = stack[--stackSize];
				while (argc--)
				{
					acc -= stack[--stackSize];
				}
				stack[stackSize++] = acc;
				break;
			}
			case 0x08: //toss
			{
				acc = stack[--stackSize];
				break;
			}
			case 0x09: //dup
			{
				acc = stack[--stackSize];
				stack[stackSize++] = acc;
				stack[stackSize++] = acc;
				break;
			}
			case 0x0C: //pushvar
			{
				acc = scriptVariables[*pc++];
				stack[stackSize++] = acc;
				break;
			}
			case 0x0D: //pushbyte
			{
				acc = *pc++;
				stack[stackSize++] = acc;
				break;
			}
			case 0x0E: //pushword
			{
				acc = (*pc++ << 8);
				acc |= *pc++;
				stack[stackSize++] = acc;
				break;
			}
			case 0x0F: //pushptr
			{
				acc = (*pc++ << 24);
				acc |= (*pc++ << 16);
				acc |= (*pc++ << 8);
				acc |= (*pc++ << 0);
				stack[stackSize++] = acc;
				break;
			}
			case 0x10: //pushlit#
			case 0x11:
			case 0x12:
			{
				acc = cmd - 0x10;
				stack[stackSize++] = acc;
				break;
			}

			case 0x81: //print
			{
				argc = stack[--stackSize];
				acc = stack[--stackSize];
				char* po = printBuffer;
				char* pi = (char*)acc;
				//pipopapo!
				while (*pi != 0)
				{
					if (*pi == '%')
					{
						pi++;
						switch (*pi)
						{
							case '%': *po++ = '%'; break;
							case 'd': po += TEXT->Format(po, "%d", stack[--stackSize]); argc--; break;
							case 's': po += TEXT->Format(po, "%s", stack[--stackSize]); argc--; break;
							default: break;
						}
						pi++;
					}
					else
						*po++ = *pi++;
				}
				*po = 0;
				saySomething(printBuffer, (argc > 1 && stack[--stackSize]));
				break;
			}
			case 0x82: //face
			{
				argc = stack[--stackSize];
				int d = entityID;
				acc = 4;
				if (argc)
					acc = stack[--stackSize];
				if (acc < 4)
					entities[d].facing = acc;
				else if (acc == 4)
				{
					if (entities[d].x > entities[0].x)
						entities[d].facing = faceLeft;
					else if (entities[d].x < entities[0].x)
						entities[d].facing = faceRight;
					else
					{
						if (entities[d].y > entities[0].y)
							entities[d].facing = faceUp;
						else if (entities[d].y < entities[0].y)
							entities[d].facing = faceDown;
					}
				}
				drawEntity((MapEntity*)&entities[d]);
				break;
			}
			case 0x83: //ask
			{
				argc = stack[--stackSize];
				//sprintf(printBuffer, "pb $%x", printBuffer); saySomething(printBuffer, 0);
				printBuffer[0] = 0;
				char *biga = printBuffer;
				char *chu;
				int opts = 0;
				while (argc--)
				{
					chu = (char*)stack[--stackSize];
					strcpy(biga, chu);
					biga += strlen(biga);
					*biga++ = 0;
					opts++;
				}
				acc = doMenu(2, 8, printBuffer, opts);
				break;
			}
			case 0x84: //portrait
			{
				argc = stack[--stackSize];
				if (argc == 0)
					acc = 0;
				else
				{
					acc = stack[--stackSize];
					argc--;
				}
				while (argc--) --stackSize;
				dialoguePortrait = acc;
				break;
			}
		}
		//printf(" 0x%04X  ", acc);
		//for (int i = 0; i < stackSize; i++)	printf("%d ", stack[i]);
		//printf("\n");
		//if (cmd == 0x06) printf("> \"%s\"\n", printBuffer);
	}
}

void entityPlayerMotor(MapEntity *entity)
{
	switch (lastInput)
	{
		case KEY_LEFT: entityWalk(entity, 1); break;
		case KEY_RIGHT: entityWalk(entity, 3); break;
		case KEY_UP: entityWalk(entity, 2); break;
		case KEY_DOWN: entityWalk(entity, 0); break;
		case 30: entity->state = stateTsk; entity->counter[0] = 0; break;
		case 31: entity->state = stateWave; entity->counter[0] = 0; break;
		case 32: entity->state = stateLaugh; entity->counter[0] = 0; break;
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
		keys[i] = 42;
//		if(Actors[i].sprite)
//		{
			keys[i] = -256 * entities[i].y + i;
			count++;
//		}
	}
	id_sort_shell(keys, ids, count);

	for (int i = 0; i < count; i++)
	{
		entities[ids[i]].oid = count - i;
		updateEntity((MapEntity*)&entities[ids[i]]);
		drawEntity((MapEntity*)&entities[ids[i]]);
	}
}

void drawWindow(int l, int t, int w, int h)
{
	for (int i = t + 1; i < t + h - 1; i++)
	{
		MAP4[(i * 64) + l] = 0xF30A;
		MAP4[(i * 64) + l + w - 1] = 0xF30C;
		for (int j = l + 1; j < l + w - 1; j++)
			MAP4[(i * 64) + j] = 0xF301;
	}
	for (int i = l + 1; i < l + w - 1; i++)
	{
		MAP4[(t * 64) + i] = 0xF30B;
		MAP4[((t + h - 1) * 64) + i] = 0xF30D;
	}
	MAP4[(t * 64) + l] = 0xF306;
	MAP4[(t * 64) + l + w - 1] = 0xF307;
	MAP4[((t + h - 1) * 64) + l] = 0xF308;
	MAP4[((t + h - 1) * 64) + l + w - 1] = 0xF309;
}

void eraseWindow(int l, int t, int w, int h)
{
	for (int i = t; i < t + h; i++)
	{
		for (int j = l; j < l + w; j++)
			MAP4[(i * 64) + j] = 0;
	}
}

void drawString(int x, int y, const char* string)
{
	char *c = (char*)string;
	int pos = (y * 64) + x;
	int t = 0;
	while (*c)
	{
		if (*c == '\n')
		{
			y += 2;
			pos = (y * 64) + x;
			c++;
			continue;
		}
		t = (*c - ' ' + 16) * 2;
		t |= 0xF300;
		MAP4[pos +  0] = t;
		MAP4[pos + 64] = t + 1;
		pos++;
		c++;
	}
}

int main(void)
{
	REG_MAPSET = 0xF0;

	MISC->SetTextMode(SMODE_TILE);
	MISC->DmaClear(TILESET, 0, 0x4000, DMA_INT);
	MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);
	MISC->DmaCopy(PALETTE + 240, (int16_t*)&fontPal, 8, DMA_INT);
	MISC->DmaCopy(PALETTE + 256 + 240, (int16_t*)&fontPal, 8, DMA_INT);

//	MISC->DmaCopy(TILESET + 0x6000, (int8_t*)&fontTiles, 0x700, DMA_INT);

	MISC->DmaCopy(TILESET + 0x6000, (int8_t*)&fontTiles, 0x80, DMA_INT);
	MISC->DmaClear(TILESET+ 0x6400, 0x88888888, 0x5D0, DMA_INT);
//	MISC->DmaCopy(TILESET + 0x6400, (int8_t*)&uiTiles + 0x400, 0x5D0, DMA_INT);
	REG_BLITSOURCE = (int32_t)fontTiles + 0x400;
	REG_BLITTARGET = (int32_t)TILESET + 0x6400;
	REG_BLITLENGTH = 0x1740;
	REG_BLITKEY = 0;
	REG_BLITCONTROL = BLIT_COPY | 0xC0 | BLIT_COLORKEY;

	MISC->DmaCopy(PALETTE + 256, (int16_t*)spritepalPal, 48, DMA_INT);

	MISC->DmaClear(MAP1, 0, WIDTH * HEIGHT, DMA_INT);
	MISC->DmaClear(MAP2, 0, WIDTH * HEIGHT, DMA_INT);
	MISC->DmaClear(MAP3, 0, WIDTH * HEIGHT, DMA_INT);
	MISC->DmaClear(MAP4, 0, WIDTH * HEIGHT, DMA_INT);

	REG_HDMASOURCE[0] = (int32_t)uiBackground;
	REG_HDMATARGET[0] = (int32_t)PALETTE + 496;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	map = (Map*)testMap;
	MISC->DmaCopy(TILESET + 0x2000, map->tileset->graphics, 0x1000, DMA_INT);
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
				break;
			default: me->motor = 0; break;
		}
		data += 10;
	}
	PALETTE[0] = 0;

	drawMap();
	//saySomething("* What a year, huh?\n* Captain, it's February.", 1);
	//saySomething("0123456789012345678901234567890123\b\nSupercalifragilisticexpialidocious\nEven though the sound of it is\nsomething quite atrocious.", 0);

	for(;;)
	{
		updateAndDraw();
		vbl();
		getInput();
	}
}
