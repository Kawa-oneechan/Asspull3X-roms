#include "../ass.h"
IBios* interface;

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0

#define WIDTH 40
#define HEIGHT 30

#define MAP ((uint16_t*)0x0E000000)

extern const uint16_t hdma1[];
extern const uint16_t tilesTiles[256];
extern const uint16_t tilesPal[16];

int score = 0, fruitTimer = 0;

typedef struct
{
	int x;
	int y;
} pos;
pos fruit;

char spaces[WIDTH * HEIGHT] = {0};

struct s_node
{
	void *value;
	struct s_node *prev;
	struct s_node *next;
} *front = NULL, *back = NULL;
typedef struct s_node node;

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

void WaitForKey()
{
	while (REG_KEYIN == 0);
	vbl();
	while (REG_KEYIN != 0);
}

void* Peek()
{
	return front == NULL ? NULL : front->value;
}

void* Dequeue()
{
	node *oldfront = front;
	front = front->next;
	return oldfront->value;
}

void Enqueue(pos position)
{
   pos *newpos   = (pos*)malloc(sizeof(position));
   node *newnode = (node*)malloc(sizeof(node));

   newpos->x = position.x;
   newpos->y = position.y;
   newnode->value = newpos;

   if (front == NULL && back == NULL)
	   front = back = newnode;
   else
   {
	   back->next = newnode;
	   newnode->prev = back;
	   back = newnode;
   }
}

void Tile(int y, int x, uint16_t tile)
{
	MAP[(y * 64) + x] = tile;
}

void Write(int y, int x, char* str)
{
	char *b = str;
	while (*b)
	{
		if (*b >= '0' && *b < '9')
			Tile(y, x++, *b - '0' + 8);
		//((int16_t*)MEM_VRAM)[pos] = (*b << 8) | attribs;
		b++;
	}
}

void DrawBoard()
{
	int i;
	Tile(0, 0, 18);
	Tile(0, WIDTH - 1, 19);
	Tile(HEIGHT - 1, 0, 20);
	Tile(HEIGHT - 1, WIDTH - 1, 21);
	for (i = 1; i < HEIGHT - 1; i++)
	{
		Tile(i, 0, 23);
		Tile(i, WIDTH - 1, 23);
	}
	for (i = 1; i < WIDTH - 1; i++)
	{
		Tile(0, i, 22);
		Tile(HEIGHT - 1, i, 22);
	}
	//Write(HEIGHT - 1, 3, 0x0B, " Score: ");
}

void GameOver()
{
	DRAW->FadeToWhite();
	while(front)
	{
		node *n = front;
		front = front->next;
		free(n);
	}
	while(1);
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
	Enqueue(head);
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
		pos *tail = Dequeue();
		spaces[CoordinateToIndex(*tail)] = 0;
		Tile(tail->y, tail->x, 0);
	}

	//Draw the new head
	Tile(head.y, head.x, 2);

	char buffer[25];
	TEXT->Format(buffer, "%d", score);
	Write(HEIGHT - 1, 2, buffer);
}

int main(void)
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_TILE | SMODE_320 | SMODE_240);
	MISC->DmaCopy(PALETTE, (int8_t*)&tilesPal, 16, DMA_INT);
	MISC->DmaCopy((int8_t*)0x0E080000, (int8_t*)&tilesTiles, 1024, DMA_INT);
	MISC->DmaClear(MAP, 0, WIDTH * HEIGHT, 2);
	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	int key = KEY_RIGHT;

	MIDI_PROGRAM(1, MIDI_ACOUSTICBASS);
	MIDI_PROGRAM(2, MIDI_FX4ATMOSPHERE);
	MIDI_PROGRAM(3, MIDI_GUNSHOT);

	DrawBoard();
	PlaceAndDrawFruit();
	pos head = { 5, 5 };
	Enqueue(head);

	DRAW->FadeFromBlack();

	while(1)
	{
		vbl();
		vbl();
		vbl();
		vbl();
		vbl();
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
			GameOver();
		}
		else
		{
			MIDI_KEYON(1, (MIDI_C2 + (rand() % 3)), (40 + (rand() % 10)));
			MovePlayer(head);
		}
	}
	GameOver();

}
