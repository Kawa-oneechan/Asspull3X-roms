#include "../ass.h"
IBios* interface;

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0

#define WIDTH 40
#define HEIGHT 30

extern const uint16_t hdma1[], titleMap[];
extern const uint16_t tilesTiles[256];
extern const uint16_t tilesPal[16];

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


unsigned long score = 0;
int fruitTimer = 0;
int delay = 16;

typedef struct
{
	char x;
	char y;
} pos;
pos fruit;

char spaces[WIDTH * HEIGHT] = {0};

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

void ultoa(unsigned long val, char *buf)
{
	char *p;
	char *firstdig;
	char temp;
	unsigned int digval;
	p = buf;
	firstdig = p;
	do
	{
		digval = (unsigned)(val % 10);
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
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0 && REG_JOYPAD == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
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
			interface->VBlank = 0;
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
	uint16_t* dst = MAP1;
	uint16_t* src = (uint16_t*)titleMap;
	for (int line = 0; line < 30; line++)
	{
		for (int row = 0; row < 40; row++)
			*dst++ = *src++;
		dst += 24;
	}
	DRAW->FadeFromBlack();

	interface->VBlank = TitleMusic;

	WaitForKey();

	delay = 10;
	for (int i = 14; i < 28; i++)
		Tile(14, i, 0);
	Write(14, 15, "SPEED");
	while (1)
	{
		vbl();
		if (REG_KEYIN == KEY_UP && delay < 16)
		{
			delay++;
			while (REG_KEYIN != 0) { vbl(); }
		}
		else if (REG_KEYIN == KEY_DOWN && delay > 1)
		{
			delay--;
			while (REG_KEYIN != 0) { vbl(); }
		}
		else if (REG_KEYIN == 0x1C)
			break;
		char buffer[25];
		ultoa(delay, buffer);
		Tile(14, 23, 0);
		Write(14, 22, buffer);
	}

	//interface->VBlank = 0;
	DRAW->FadeToBlack();
	rndseed = REG_TICKCOUNT;
}

#define MAXSNAKEBITS 1024
pos snakeBits[MAXSNAKEBITS];
int headCursor = 0, tailCursor = 0;

void Tile(int y, int x, uint16_t tile)
{
	MAP1[(y * 64) + x] = tile;
}

void Write(int y, int x, char* str)
{
	char *b = str;
	while (*b)
	{
		Tile(y, x++, *b);
		b++;
	}
}

void DrawBoard()
{
	int i;

	for (i = 0; i < WIDTH * HEIGHT; i++)
		MAP1[i] = 0;

	Tile(0, 0, 16);
	Tile(0, WIDTH - 1, 17);
	Tile(HEIGHT - 1, 0, 18);
	Tile(HEIGHT - 1, WIDTH - 1, 19);
	for (i = 1; i < HEIGHT - 1; i++)
	{
		Tile(i, 0, 21);
		Tile(i, WIDTH - 1, 21);
	}
	for (i = 1; i < WIDTH - 1; i++)
	{
		Tile(0, i, 20);
		Tile(HEIGHT - 1, i, 20);
	}

	for (i = 3; i < 7; i++)
		Tile(HEIGHT - 1, i, i - 3 + 97);
	Tile(HEIGHT -1, 7, '0');
}

void GameOver()
{
	REG_OPLOUT = 0xA257;
	REG_OPLOUT = 0xB221;
//	REG_OPLOUT = 0xA357;
//	REG_OPLOUT = 0xB321;
	DRAW->FadeToWhite();
	//while(1);
	score = 0;
	WaitForKey();
	REG_OPLOUT = 0xA257;
	REG_OPLOUT = 0xB201;
}

int InBounds(pos position)
{
	return position.y < HEIGHT - 1 && position.y > 0 && position.x < WIDTH - 1 && position.x > 0;
}

int CoordinateToIndex(pos position)
{
	return WIDTH * position.y + position.x;
}

pos IndexToCoordinate(int index)
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

void MovePlayer(pos head)
{
	//Check if we ran into ourself
	int idx = CoordinateToIndex(head);
	if (spaces[idx])
		GameOver();
	spaces[idx] = 1; //Mark the space as occupied

	snakeBits[headCursor].x = head.x;
	snakeBits[headCursor].y = head.y;
	headCursor++;
	if (headCursor == MAXSNAKEBITS) headCursor = 0;

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
		tailCursor++;
		if (tailCursor == MAXSNAKEBITS) tailCursor = 0;
		spaces[CoordinateToIndex(*tail)] = 0;
		Tile(tail->y, tail->x, 0);
	}

	//Draw the new head
	Tile(head.y, head.x, 96);

	char buffer[25];
	ultoa(score, buffer);
	Write(HEIGHT - 1, 7, buffer);
}

void ClearBoard()
{
	for (int i = 0; i < WIDTH * HEIGHT; i++)
		spaces[i] = 0;
	for (int i = 0; i < 64 * 32; i++)
		MAP1[i] = 0;
}

int main(void)
{
	MISC->SetTextMode(SMODE_TILE | SMODE_320 | SMODE_240);
	MISC->DmaCopy(PALETTE, (int8_t*)&tilesPal, 16, DMA_INT);
	MISC->DmaCopy(TILESET, (int8_t*)&tilesTiles, 1024, DMA_INT);
	MISC->DmaClear(MAP1, 0, WIDTH * HEIGHT, 2);
	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);
	REG_MAPSET = 0x10;

	for (int i = 0; i < 10 * 6; i++)
		REG_OPLOUT = oplSetup[i];

	TitleScreen();

	while(1)
	{
		ClearBoard();
		int key = KEY_RIGHT;
		headCursor = 0;
		tailCursor = 0;
		DrawBoard();
		PlaceAndDrawFruit();
		pos head = { 5, 5 };
		snakeBits[headCursor].x = head.x;
		snakeBits[headCursor].y = head.y;
		headCursor++;
		if (headCursor == MAXSNAKEBITS) headCursor = 0;

		DRAW->FadeFromBlack();

		while(1)
		{
			for (int i = 0; i < delay; i++)
				vbl();

			if (fruitTimer)
			{
				if (--fruitTimer == 0)
					REG_OPLOUT = 0xB005;
			}

			int in = REG_KEYIN;
			if (REG_JOYPAD & 1) in = KEY_UP;
			else if (REG_JOYPAD & 2) in = KEY_RIGHT;
			else if (REG_JOYPAD & 4) in = KEY_DOWN;
			else if (REG_JOYPAD & 8) in = KEY_LEFT;
			rndseed += in;

			switch (in)
			{
				case KEY_DOWN:
					if (key != KEY_UP) key = in;
					break;
				case KEY_RIGHT:
					if (key != KEY_LEFT) key = in;
					break;
				case KEY_UP:
					if (key != KEY_DOWN) key = in;
					break;
				case KEY_LEFT:
					if (key != KEY_RIGHT) key = in;
					break;
			}
			switch(key)
			{
				case KEY_DOWN:
					head.y++;
					break;
				case KEY_RIGHT:
					head.x++;
					break;
				case KEY_UP:
					head.y--;
					break;
				case KEY_LEFT:
					head.x--;
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
					REG_OPLOUT = 0xA1 | beat[drum2 % 4];
					REG_OPLOUT = 0xB109;
				} else if (drum == 2)
				{
					drum2++;
					REG_OPLOUT = 0xA100 | beat[drum2 % 4];
					REG_OPLOUT = 0xB129;
				}
				else if (drum == 3)
					drum = 0;

				MovePlayer(head);
			}
		}
	}
	GameOver();

}
