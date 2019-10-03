#include "../ass.h"
#include "../ass-std.h"
IBios* interface;

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0

#define g_width 80
#define g_height 30

int g_score = 0;

typedef struct
{
	int x;
	int y;
} pos;
pos fruit;

char spaces[g_width * g_height] = {0};

struct s_node
{
	pos *position; //TODO: make this a void pointer for generality.
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

pos* peek()
{
	return front == NULL ? NULL : front->position;
}

pos* Dequeue()
{
	node *oldfront = front;
	front = front->next;
	return oldfront->position;
}

void Enqueue(pos position)
{
   pos *newpos   = (pos*)malloc(sizeof(position));
   node *newnode = (node*)malloc(sizeof(node));

   newpos->x = position.x;
   newpos->y = position.y;
   newnode->position = newpos;

   if (front == NULL && back == NULL)
	   front = back = newnode;
   else
   {
	   back->next = newnode;
	   newnode->prev = back;
	   back = newnode;
   }
}

void Write(int y, int x, char attribs, char* str)
{
	int pos = (80 * y) + x;
	char *b = str;
	while (*b)
	{
		((int16_t*)MEM_VRAM)[pos] = (*b << 8) | attribs;
		b++;
		pos++;
	}
}

void snake_draw_board()
{
	int i;
	for (i=0; i<g_height; i++)
	{
		Write(i, 0,         0x0E, "\x11\x11");
		Write(i, g_width-2, 0x0E, "\x11\x11");
	}
	for (i=1; i<g_width-1; i++)
	{
		Write(0, i,          0x0E, "\x11");
		Write(g_height-1, i, 0x0E, "\x11");
	}
	Write(g_height-1, 3, 0x0B, " Score: ");
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
	return position.y < g_height - 1 && position.y > 0 && position.x < g_width - 1 && position.x > 0;
}

int CoordinateToIndex(pos position)
{
	return g_width * position.y + position.x;
}

pos IndexToCoordinate(int index)
{
	int x = index % g_width;
	int y = index / g_width;
	return (pos) { x, y };
}

void PlaceAndDrawFruit()
{
	int idx;
	do
	{
		idx = rand() % (g_width * g_height);
		fruit = IndexToCoordinate(idx);
	}
	while(spaces[idx] || !InBounds(fruit));
	Write(fruit.y, fruit.x, 0x1C, "\x02");
}

void MovePlayer(pos head)
{
	//Check if we ran into ourself
	int idx = CoordinateToIndex(head);
	if (spaces[idx])
		GameOver();
	spaces[idx] = 1; //Mark the space as occupied
	Enqueue(head);
	g_score += 10;

	//Check if we're eating the fruit
	if (head.x == fruit.x && head.y == fruit.y)
	{
		Write(fruit.y, fruit.x, 0x12, "!");
		PlaceAndDrawFruit();
		g_score += 1000;
	}
	else
	{
		//Handle the tail
		pos *tail = Dequeue();
		spaces[CoordinateToIndex(*tail)] = 0;
		Write(tail->y, tail->x, 0x10, " ");
	}

	//Draw the new head
	Write(head.y, head.x, 0x1A, "\x83");

	char buffer[25];
	sprintf(buffer, "%d ", g_score);
	Write(g_height-1, 11, 0x0A, buffer);
}

int main(void)
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(1, 15);
	TEXT->ClearScreen();
	TEXT->SetCursorPosition(0, 0);

	int key = KEY_RIGHT;

	snake_draw_board();
	PlaceAndDrawFruit();
	pos head = { 5, 5 };
	Enqueue(head);

	while(1)
	{
		vbl();
		vbl();
		vbl();
		vbl();
		vbl();
		vbl();

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
			GameOver();
		else
			MovePlayer(head);
	}
	GameOver();

}
