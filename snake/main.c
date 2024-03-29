#include "../ass.h"
#include "../ass-keys.h"
IBios* interface;

#define WIDTH 40
#define HEIGHT 30

extern const uint16_t titleMap[], bgMap[];
extern const uint16_t tilesTiles[256], bgTiles[256];
extern const uint16_t tilesPal[16], bgPal[16];

const uint16_t oplSetup[] = {
//fupp, apple, F1
	0x2037, 0x4000, 0x6059, 0x80FF, 0xE000, 0x2372, 0x4300, 0x6348, 0x83FF, 0xE300,
//jungdrum, movement beat, F3EDC
	0x2132, 0x4144, 0x61F8, 0x81FF, 0xE100, 0x2411, 0x440F, 0x64F5, 0x847F, 0xE400,
//ultra, death, combined with snrsust, C1
	0x2200, 0x4200, 0x6274, 0x82F9, 0xE200, 0x2501, 0x4500, 0x6573, 0x85F9, 0xE500,
//snrsust, death, combined with ultra, C1
	0x2806, 0x4800, 0x68F0, 0x88F0, 0xE800, 0x2BC4, 0x4B03, 0x6BC4, 0x8B34, 0xEB00,
//elpiano1, title song, C4DEDCDE.C.C.
	0x2901, 0x494F, 0x69F1, 0x8950, 0xE900, 0x2C01, 0x4C04, 0x6CD2, 0x8C7C, 0xEC00,
//Bass 295, alternative death sound, C2
	0x2AD0, 0x4AC8, 0x6A84, 0x8AF2, 0xEA05, 0x2D01, 0x4D00, 0x6DA3, 0x8D02, 0xED04,
};

const uint8_t beat[] = {
	//F     E     D     C
	0xC4, 0xB0, 0x81, 0x57
	//0xA100 | beat[drum]
	//0xB12D
};
uint8_t drum = 0, drum2 = 0;


uint32_t score = 0;
int fruitTimer = 0;
int delay = 10;

typedef struct
{
	char x;
	char y;
} pos;
pos fruit;

char spaces[WIDTH * HEIGHT] = {0};

uint32_t rndseed = 0xDEADBEEF;

void srand(uint32_t seed)
{
	rndseed = seed;
}

uint32_t rand()
{
	rndseed = (rndseed * 0x41C64E6D) + 0x6073;
	return rndseed;
}

void ultoa(uint32_t val, char *buf)
{
	char *p;
	char *firstdig;
	char temp;
	uint8_t digval;
	p = buf;
	firstdig = p;
	do
	{
		digval = (uint8_t)(val % 10);
		val /= 10;
		*p++ = (char)(digval + '0');
	} while (val > 0);
	*p-- = '\0';
	do
	{
		temp = *p;
		*p = *firstdig;
		*firstdig = temp;
		--p;
		++firstdig;
	} while (firstdig < p);
}

void WaitForKey()
{
	while (INP_KEYIN == 0 && INP_JOYPAD1 == 0) { vbl(); }
}

void TitleMusic()
{
	static uint8_t cursor = 0, timer = 0, lastPitch = 0;
	const uint8_t tune[] = { 0x57, 0x81, 0xB0, 0x81, 0x57, 0x81, 0xB0, 0x57, 0x57 };
	if (timer == 2)
	{
		REG_OPLOUT = 0xA400 | lastPitch;
		REG_OPLOUT = 0xB411;
	}
	if (timer == 0)
	{
		lastPitch = tune[cursor++];
		timer = cursor > 6 ? 16 : 8;
		if (cursor == 10)
		{
			interface->vBlank = 0;
			return;
		}
		if (lastPitch != 0)
		{
			REG_OPLOUT = 0xA400 | lastPitch;
			REG_OPLOUT = 0xB431;
		}
	}
	else
		timer--;
}

void Tile(int, int, uint16_t);
void Write(int, int, char*);
void TitleScreen()
{
	REG_SCREENFADE = 31;
	uint16_t* dst = MAP2;
	uint16_t* src = (uint16_t*)titleMap;
	for (int line = 0; line < 30; line++)
	{
		for (int row = 0; row < 40; row++)
			*dst++ = *src++;
		dst += 24;
	}
	DRAW->Fade(true, false);

	interface->vBlank = TitleMusic;

	WaitForKey();

	for (int i = 14; i < 28; i++)
		Tile(14, i, 0);
	Write(14, 15, "SPEED");
	while (1)
	{
		vbl();
		int dpadbuts = INP_JOYPAD1;
		int key = INP_KEYIN;
		if ((key == KEYSCAN_UP || dpadbuts & BUTTON_UP) && delay < 16)
		{
			while (INP_KEYIN || INP_JOYPAD1) vbl();
			delay++;
		}
		else if ((key == KEYSCAN_DOWN || dpadbuts & BUTTON_DOWN) && delay > 1)
		{
			while (INP_KEYIN || INP_JOYPAD1) vbl();
			delay--;
		}
		else if (key == KEYSCAN_ENTER || dpadbuts & BUTTON_A)
			break;
		char buffer[25];
		ultoa(delay, buffer);
		Tile(14, 23, 0);
		Write(14, 22, buffer);
	}

	//interface->vBlank = 0;
	DRAW->Fade(false, false);
	rndseed = REG_TICKCOUNT;
}

#define MAXSNAKEBITS 1024
pos snakeBits[MAXSNAKEBITS];
int headCursor = 0, tailCursor = 0;
int headDir;

void Tile(int y, int x, uint16_t tile)
{
	MAP2[(y * 64) + x] = tile;
}

void Write(int y, int x, char* str)
{
	char *b = str;
	while (*b)
	{
		Tile(y, x++, *b | 0x1000);
		b++;
	}
}

void DrawBoard()
{
	int i;

	for (i = 0; i < WIDTH * HEIGHT; i++)
		MAP2[i] = 0;

	Tile(0, 0, 16 | 0x1000);
	Tile(0, WIDTH - 1, 17 | 0x1000);
	Tile(HEIGHT - 1, 0, 18 | 0x1000);
	Tile(HEIGHT - 1, WIDTH - 1, 19 | 0x1000);
	for (i = 1; i < HEIGHT - 1; i++)
	{
		Tile(i, 0, 21 | 0x1000);
		Tile(i, WIDTH - 1, 21 | 0x1000);
	}
	for (i = 1; i < WIDTH - 1; i++)
	{
		Tile(0, i, 20 | 0x1000);
		Tile(HEIGHT - 1, i, 20 | 0x1000);
	}

	for (i = 3; i < 7; i++)
		Tile(HEIGHT - 1, i, (i - 3 + 97) | 0x1000);
	Tile(HEIGHT -1, 7, '0' | 0x1000);
}

void GameOver()
{
	REG_OPLOUT = 0xA257;
	REG_OPLOUT = 0xB221;
//	REG_OPLOUT = 0xA357;
//	REG_OPLOUT = 0xB321;
	DRAW->Fade(false, true);
	//while(1);
	score = 0;
	WaitForKey();
	REG_OPLOUT = 0xA257;
	REG_OPLOUT = 0xB201;
}

static int InBounds(pos position)
{
	return position.y < HEIGHT - 1 && position.y > 0 && position.x < WIDTH - 1 && position.x > 0;
}

static int CoordinateToIndex(pos position)
{
	return WIDTH * position.y + position.x;
}

static pos IndexToCoordinate(int index)
{
	int x = index % WIDTH;
	int y = index / WIDTH;
	return (pos) { x, y };
}

void PlaceAndDrawFruit()
{
	int idx;
	do
	{
		idx = rand() % (WIDTH * HEIGHT);
		fruit = IndexToCoordinate(idx);
	}
	while(spaces[idx] || !InBounds(fruit));
	Tile(fruit.y, fruit.x, 1);
}

bool MovePlayer(pos head)
{
	//Check if we ran into ourself
	int idx = CoordinateToIndex(head);
	if (spaces[idx])
		return true;
	spaces[idx] = 1; //Mark the space as occupied

	snakeBits[headCursor].x = head.x;
	snakeBits[headCursor].y = head.y;
	headCursor++;
	headCursor %= MAXSNAKEBITS;

	score += 10;

	//Check if we're eating the fruit
	if (head.x == fruit.x && head.y == fruit.y)
	{
		REG_OPLOUT = 0xA0C4; //F1 -> 0x1CA, block 1
		REG_OPLOUT = 0xB025;
		fruitTimer = 3;
		PlaceAndDrawFruit();
		score += 1000;
	}
	else
	{
		//Handle the tail
		pos *tail = &snakeBits[tailCursor];
		if (tail->x)
		{
			spaces[CoordinateToIndex(*tail)] = 0;
			Tile(tail->y, tail->x, 0);
		}
		tail->x = tail->y = 0;
		tailCursor++;
		tailCursor %= MAXSNAKEBITS;
	}

	//Draw the new head
	//Tile(head.y, head.x, 96);

	const uint16_t headDirs[] =  { 96, 9, 11, 8, 10 };
	pos *here, *next;
	uint16_t lastDir = 0;
	for (int i = tailCursor; i < headCursor; i++)
	{
		int j = i + 1;
		i %= MAXSNAKEBITS;
		j %= MAXSNAKEBITS;
		here = &snakeBits[i];
		next = &snakeBits[j];
		uint16_t there = MAP2[(here->y * 64) + here->x];
		uint16_t tnext = 96;
		if (next->x > here->x) // going right
		{
			if (there == 2) //||
				there = (lastDir == 2) ? 6 : 4; //|_ or .-
			lastDir = 1;
			tnext = 3; //==
			if (i == tailCursor)
				there = 12;
		}
		else if (next->y > here->y) // going down
		{
			if (there == 3) //==
				there = (lastDir == 1) ? 5 : 4; //-. or .-
			lastDir = 2;
			tnext = 2; //||
			if (i == tailCursor)
				there = 14;
		}
		else if (next->x < here->x) // going left
		{
			if (there == 2) //||
				there = (lastDir == 2) ? 7 : 5; //_| or -.
			lastDir = 3;
			tnext = 3; //==
			if (i == tailCursor)
				there = 13;
		}
		else if (next->y < here->y) // going up
		{
			if (there == 3) //==
				there = (lastDir == 1) ? 7 : 6; //_| or |_
			lastDir = 4;
			tnext = 2; //||
			if (i == tailCursor)
				there = 15;
		}
		if (here->x) Tile(here->y, here->x, there);
		if (next->x) Tile(next->y, next->x, tnext);
	}
	Tile(head.y, head.x, headDirs[headDir]);

	char buffer[25];
	ultoa(score, buffer);
	Write(HEIGHT - 1, 7, buffer);

	return false;
}

void ClearBoard()
{
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		spaces[i] = 0;
	for (int i = 0; i < 64 * 32; i++)
		MAP2[i] = 0;
	for (int i = 0; i < MAXSNAKEBITS; i++)
		snakeBits[i].x = snakeBits[i].y = 0;
	REG_SCREENFADE = 0;
}

int main(void)
{
	REG_SCREENMODE = SMODE_TILE;
	MISC->DmaCopy(PALETTE, (int8_t*)&tilesPal, 32, DMA_SHORT);
	MISC->DmaCopy(PALETTE + 32, (int8_t*)&bgPal, 16, DMA_SHORT);
	MISC->DmaCopy(TILESET, (int8_t*)&tilesTiles, 1024, DMA_INT);
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&bgTiles, 256, DMA_INT);
	MISC->DmaClear(MAP1, 0, WIDTH * HEIGHT, 2);
	MISC->DmaClear(MAP2, 0, WIDTH * HEIGHT, 2);
	REG_MAPSET = 0x30;

	for (int i = 0; i < 10 * 6; i++)
		REG_OPLOUT = oplSetup[i];

	uint16_t* dst = MAP1;
	uint16_t* src = (uint16_t*)bgMap;
	for (int line = 0; line < 30; line++)
	{
		for (int row = 0; row < 40; row++)
			*dst++ = (*src++ | 0x2000) + 256;
		dst += 24;
	}

	TitleScreen();

	while(1)
	{
		ClearBoard();
		int key = KEYSCAN_RIGHT;
		headCursor = 4;
		tailCursor = 0;
		DrawBoard();
		PlaceAndDrawFruit();
		pos head = { 5, 5 };
		snakeBits[headCursor].x = head.x;
		snakeBits[headCursor].y = head.y;
		headCursor++;
		headCursor %= MAXSNAKEBITS;

		DRAW->Fade(true, false);

		while(1)
		{
			for (int i = 0; i < delay; i++)
				vbl();

			if (fruitTimer)
			{
				if (--fruitTimer == 0)
					REG_OPLOUT = 0xB005;
			}

			int dpadbuts = INP_JOYPAD1;
			int keyIn = INP_KEYIN;
			if ((keyIn == KEYSCAN_UP || dpadbuts & BUTTON_UP) && key != KEYSCAN_DOWN) key = KEYSCAN_UP;
			else if ((keyIn == KEYSCAN_DOWN || dpadbuts & BUTTON_DOWN) && key != KEYSCAN_UP) key = KEYSCAN_DOWN;
			if ((keyIn == KEYSCAN_RIGHT || dpadbuts & BUTTON_RIGHT) && key != KEYSCAN_LEFT) key = KEYSCAN_RIGHT;
			else if ((keyIn == KEYSCAN_LEFT || dpadbuts & BUTTON_LEFT) && key != KEYSCAN_RIGHT) key = KEYSCAN_LEFT;
			switch(key)
			{
				case KEYSCAN_RIGHT:
					headDir = 1;
					head.x++;
					break;
				case KEYSCAN_DOWN:
					headDir = 2;
					head.y++;
					break;
				case KEYSCAN_LEFT:
					headDir = 3;
					head.x--;
					break;
				case KEYSCAN_UP:
					headDir = 4;
					head.y--;
					break;
			}
			if (!InBounds(head))
			{
				GameOver();
				break;
			}
			else
			{
				if (++drum == 1)
				{
					REG_OPLOUT = 0xA100 | beat[drum2 % 4];
					REG_OPLOUT = 0xB109;
				} else if (drum == 2)
				{
					drum2++;
					REG_OPLOUT = 0xA100 | beat[drum2 % 4];
					REG_OPLOUT = 0xB129;
				}
				else if (drum == 3)
					drum = 0;

				if (MovePlayer(head))
				{
					GameOver();
					break;
				}
			}
		}
	}
	GameOver();

}
