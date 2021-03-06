#include "../ass.h"
IBios* interface;

extern const uint16_t tilesTiles[], tilesPal[];
extern const uint16_t girl1Tiles[], girl1Pal[], girl1Map[];

#define WELL_WIDTH 6
#define WELL_HEIGHT 12

#define WIDTH 40
#define HEIGHT 30

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0

#define POOF 12

#define SPRITEA_BUILD(t,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &	1) << 11) |		\
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

char map[WELL_WIDTH*WELL_HEIGHT] = {0};
char mapCopy[WELL_WIDTH*WELL_HEIGHT] = {0};
char current[3] = {1,2,3};
int currentX = 0, currentSub = 0, currentY = 0;

static const int tilePals[] = {
	0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int rndseed = 0xDEADBEEF;

void srand(unsigned int seed)
{
	rndseed = seed;
}

unsigned int rand()
{
	rndseed = (rndseed * 0x41C64E6D) + 0x6073;
	return rndseed;
}

void drawTile(int col, int row, int tile)
{
	int pos = (row * 128) + (col * 2) + (14 + 3 * 64);
	int pal = tilePals[tile] << 12;
	int t = pal | (tile * 4);
	MAP4[pos] = t + 0;
	MAP4[pos + 1] = t + 1;
	MAP4[pos + 64] = t + 2;
	MAP4[pos + 65] = t + 3;
}

void draw()
{
	char* here = map;
	for (int row = 0; row < WELL_HEIGHT; row++)
		for (int col = 0; col < WELL_WIDTH; col++)
			drawTile(col, row, *here++);
}

void place(int col, int row, int tile)
{
	if (row < 0) return;
	map[(row * WELL_WIDTH) + col] = tile;
}

void dropNew()
{
	for (int i = 0; i < 3; i++)
		current[i] = 1 + (rand() % 3);

	//don't cheapen out on me here
	while (current[0] == current[1] && current[1] == current[2])
		current[rand() % 3] = 1 + (rand() % 3);

	currentX = 3;
	currentY = 2;
	currentSub = 0;

	if (map[(currentY * WELL_WIDTH) + currentX])
	{
		//oh fuck
		//GameOver();
	}
}

int applyGravity()
{
	int x, y, cpt;

	for (x = 0; x < WELL_WIDTH * WELL_HEIGHT; x++)
		map[x] = 0;

	for (x = 1; x < WELL_WIDTH - 1; x++)
	{
		cpt = 0;
		// remove the empty slots of each columns and make fall the pieces
		for (y = WELL_HEIGHT - 2; y > 1; y--) {
			if (mapCopy[x + y * WELL_WIDTH] != POOF)
			{
				map[x + (WELL_HEIGHT - 2 - cpt) * WELL_WIDTH] = mapCopy[x + y * WELL_WIDTH];
				cpt++;
			}
		}
	}
	draw();

	return 0;
}

void checkTriplets()
{
	int x, y, here, retcode;
	int differences;

checkAgain:
	differences = 0;

	for (x = 0; x < WELL_WIDTH * WELL_HEIGHT; x++)
		mapCopy[x] = map[x];

	for (y = 0; y < WELL_HEIGHT; y++)
	{
		for (x = 0; x < WELL_WIDTH; x++)
		{
			here = map[x + y * WELL_WIDTH];

			//----------------------------------------------
			// Check for valid combinations
			//----------------------------------------------
			// we start on 2 because the UP and LEFT tests will test the 0 &
			// 1 blocks when testing the block number 2 for a valid combination
			// Up-left and up-right tests will detect down-left & down-right
			if (here < POOF && here != 0)
			{
				if (y >= 2)
				{
					// check for up combinations
					if (here == map[x + (y-1) * WELL_WIDTH] && here == map[x + (y-2) * WELL_WIDTH])
					{
						mapCopy[x + (y-0) * WELL_WIDTH] = POOF;
						mapCopy[x + (y-1) * WELL_WIDTH] = POOF;
						mapCopy[x + (y-2) * WELL_WIDTH] = POOF;
						retcode++;
					}
					if (x >= 2)
					{
						// check for up - left combinations
						if (here == map[(x-1) + (y-1) * WELL_WIDTH] && here == map[(x-2) + (y-2) * WELL_WIDTH])
						{
							mapCopy[(x-0) + (y-0) * WELL_WIDTH] = POOF;
							mapCopy[(x-1) + (y-1) * WELL_WIDTH] = POOF;
							mapCopy[(x-2) + (y-2) * WELL_WIDTH] = POOF;
							retcode++;
						}
					}

				}
				if (x >= 2)
				{
					// check for left combinations
					if (here == map[(x-1) + y * WELL_WIDTH] && here == map[(x-2) + y * WELL_WIDTH])
					{
						mapCopy[(x-0) + y * WELL_WIDTH] = POOF;
						mapCopy[(x-1) + y * WELL_WIDTH] = POOF;
						mapCopy[(x-2) + y * WELL_WIDTH] = POOF;
						retcode++;
					}
				}
				if (y >= 2 && x <= WELL_WIDTH - 3)
				{
					// check for up - right combinations
					if (here == map[(x+1) + (y-1) * WELL_WIDTH] && here == map[(x+2) + (y-2) * WELL_WIDTH])
					{
						mapCopy[(x-0) + (y-0) * WELL_WIDTH] = POOF;
						mapCopy[(x+1) + (y-1) * WELL_WIDTH] = POOF;
						mapCopy[(x+2) + (y-2) * WELL_WIDTH] = POOF;
						retcode++;
					}
				}
			}
			//----------------------------------------------
			if (here >= POOF && here < POOF + 2)
			{
				mapCopy[x + y * WELL_WIDTH] = here + 1;
			}
			else if(here == POOF + 2)
			{
				mapCopy[x + y * WELL_WIDTH] = 0;
				if (y > 0)
				{
					for (int g = y; g > 0; g--)
						mapCopy[x + g * WELL_WIDTH] = mapCopy[x + (g-1) * WELL_WIDTH];
					mapCopy[x] = 0;
				}
			}
		}
	}

	for (x = 0; x < WELL_WIDTH * WELL_HEIGHT; x++)
	{
		if (map[x] != mapCopy[x])
			differences++;
		map[x] = mapCopy[x];
	}

	if (differences)
	{
		draw();
		for (int vbls = 0; vbls < 8; vbls++)
			vbl();
		goto checkAgain;
	}
	//return(retcode);
}

void checkLanding()
{
	if (currentY < WELL_HEIGHT - 1 && map[((currentY + 1) * WELL_WIDTH) + currentX] == 0)
		return;
	//We've landed. Copy the current set into the board, check for triplets, and drop a new set.
	for (int i = 0; i < 3; i++)
	{
		SPRITES_A[i] = 0;
		SPRITES_B[i] = 0;
	}
	for (int i = 0; i < 3; i++)
		place(currentX, currentY - i, current[i]);
	draw();
	checkTriplets();
	dropNew();
}

void movePlayer()
{
	static int dropTimer = 0;
	if (dropTimer != 0)
	{
		dropTimer--;
		if (REG_KEYIN != KEY_DOWN && REG_JOYPAD != 4)
			return;
	}
	dropTimer = 8;
	checkLanding();
	currentSub += 8;
	if (currentSub >= 16)
	{
		currentSub = 0;
		currentY++;
	}
}

void moveSideways(int diff)
{
	static int moveTimer = 0;
	if (moveTimer != 0)
	{
		moveTimer--;
		return;
	}
	moveTimer = 2;
	int newX = currentX + diff;
	if (newX < 0 || newX >= WELL_WIDTH)
		return;
	if (map[(currentY * WELL_WIDTH) + newX] != 0)
		return;
	currentX = newX;
}

void rotate()
{
	static int rotateTimer = 0;
	if (rotateTimer != 0)
	{
		rotateTimer--;
		return;
	}
	rotateTimer = 6;
	char t = current[2];
	current[2] = current[1];
	current[1] = current[0];
	current[0] = t;
}

void drawPlayer()
{
	int x = currentX * 16;
	int y = currentY * 16;
	y += currentSub;
	x += 14 * 8;
	y += 3  * 8;
	for (int i = 0; i < 3; i++)
	{
		SPRITES_A[i] = SPRITEA_BUILD((current[i] * 4), 1, tilePals[(int)current[i]]);
		SPRITES_B[i] = SPRITEB_BUILD(x, y, 1, 1, 0, 0, 0, 0);
		y -= 16;
	}
}

void drawFrame(int l, int t, int r, int b)
{
	MAP4[l + t * 64] = 60;
	MAP4[l + b * 64] = 62;
	MAP4[r + b * 64] = 63;
	MAP4[r + t * 64] = 64;
	for (int i = l+1; i < r; i++)
	{
		MAP4[i + t * 64] = 61;
		MAP4[i + b * 64] = 61;
	}
	for (int i = t+1; i < b; i++)
	{
		MAP4[l + i * 64] = 65;
		MAP4[r + i * 64] = 65;
	}
	for (int i = l+1; i < r; i++)
	{
		for (int j = t+1; j < b; j++)
		{
			MAP2[i + j * 64] = 90;
		}
	}
}

void drawEntirity()
{
	drawFrame(13, 2, 26, 27);
	drawFrame(28, 2, 34, 17);
	draw();
}

void WaitForKey()
{
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0 && REG_JOYPAD == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

int main(void)
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_TILE);
	MISC->DmaCopy(TILESET, (int8_t*)&tilesTiles, 96 * 8, DMA_INT);
	MISC->DmaCopy(PALETTE, (int16_t*)&tilesPal, 16, DMA_INT);
	MISC->DmaClear(MAP1, 0, WIDTH * HEIGHT, 2);
	MISC->DmaClear(MAP2, 0, WIDTH * HEIGHT, 2);
	MISC->DmaClear(MAP3, 0, WIDTH * HEIGHT, 2);
	MISC->DmaClear(MAP4, 0, WIDTH * HEIGHT, 2);
	REG_MAPSET = 0xB0;
	REG_MAPBLEND = 0x02;

	MISC->DmaCopy(TILESET + (128 * 32), (int8_t*)&girl1Tiles, 10240, DMA_INT);
	MISC->DmaCopy(PALETTE + 32, (int16_t*)&girl1Pal, 16, DMA_INT);
	PALETTE[0] = girl1Pal[0];
	for (uint32_t i = 1, t = 0; i < 29; i++)
	{
		for (int j = 4; j < 36; j++, t++)
			MAP1[j + i * 64] = (t + 128) | 0x2000;
	}

	for (int i = 1; i < WELL_WIDTH; i++)
	{
		place(i, WELL_HEIGHT - 1, i + 1);
		place(i, WELL_HEIGHT - 2, ((i + 1) % 6) + 1);
	}

	drawEntirity();
	dropNew();

	int in;
	for(;;)
	{
		drawPlayer();
		movePlayer();
		for (int delay = 0; delay < 2; delay++)
		{
			vbl();
			in = REG_KEYIN;
			if (REG_JOYPAD & 1) in = KEY_UP;
			else if (REG_JOYPAD & 2) in = KEY_RIGHT;
			else if (REG_JOYPAD & 4) in = KEY_DOWN;
			else if (REG_JOYPAD & 8) in = KEY_LEFT;
			else if (REG_JOYPAD & 16) in = KEY_UP;
			rndseed += in;
		}
		switch (in)
		{
			case KEY_LEFT: moveSideways(-1); break;
			case KEY_RIGHT: moveSideways(1); break;
			case KEY_UP: rotate(); break;
		}
	}
}
