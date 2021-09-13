#include "../ass.h"
#include "../ass-midi.h"
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
	static int cursor = 0, timer = 0, lastPitch;
	const int tune[] =
	{
		MIDI_G4, 8,
		MIDI_A4, 8,
		MIDI_B4, 8,
		MIDI_A4, 8,
		MIDI_G4, 8,
		MIDI_A4, 8,
		MIDI_B4, 16,
		MIDI_G4, 16,
		MIDI_G4, 32,
		0, 0
	};
	if (timer == 0)
	{
		MIDI_KEYOFF(1, lastPitch, 40);
		lastPitch = tune[cursor++];
		timer = tune[cursor++];
		if (lastPitch == 0)
		{
			interface->VBlank = 0;
			return;
		}
		MIDI_KEYON(1, lastPitch, 40);
	}
	else
		timer--;
	//MIDI_KEYON(1, (MIDI_C2 + (rand() % 3)), (40 + (rand() % 10)));
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
	DRAW->FadeToWhite();
	//while(1);
	score = 0;
	WaitForKey();
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
		MIDI_KEYON(2, MIDI_A5, 80);
		fruitTimer = 2;
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

	MIDI_PROGRAM(1, MIDI_ACOUSTICBASS);
	MIDI_PROGRAM(2, MIDI_FX4ATMOSPHERE);
	MIDI_PROGRAM(3, MIDI_GUNSHOT);

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
				fruitTimer--;
				if (fruitTimer == 1)
				{
					MIDI_KEYOFF(2, MIDI_A5, 80);
					MIDI_KEYON(2, MIDI_D6, 80);
				}
				if (fruitTimer == 0)
				{
					MIDI_KEYOFF(2, MIDI_D6, 80);
				}
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
				MIDI_KEYON(3, MIDI_C2, 80);
				MIDI_KEYOFF(2, MIDI_A5, 80);
				MIDI_KEYOFF(2, MIDI_D6, 80);
				GameOver();
				break;
			}
			else
			{
				MIDI_KEYON(1, (MIDI_C2 + (rand() % 3)), (40 + (rand() % 10)));
				MovePlayer(head);
			}
		}
	}
	GameOver();

}
