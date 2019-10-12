#include "../ass.h"
IBios* interface;

extern const TImageFile titlePic;
extern const uint16_t tilesTiles[], tilesPal[];
extern const uint16_t playerTiles[], playerPal[];
extern const uint16_t hdma1[];
extern const char * const levels[];

#define WIDTH 40
#define HEIGHT 30

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0

#define SPRITEA_BUILD(t,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
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

#define SPACE 0
#define BOX 1
#define GOAL 2
#define BOXINGOAL 3
#define WALL 4
#define WALLALT 5
#define EXTERIOR 6

#define BOUNDS 20
#define ANIMSPEED 32

int levelNum = 0;

char map[BOUNDS*BOUNDS] = {0};
int playerX = 0, playerY = 0;
int lastDir;

void drawPlayer()
{
	int tile = 0;
	int flip = 0;
	switch (lastDir)
	{
		case 0: //left
			break;
		case 1: //right
			flip = 1;
			break;
		case 2: //down
			tile = 16;
			break;
		case 3: //up
			tile = 32;
			break;
	}
	if (REG_TICKCOUNT % ANIMSPEED > (ANIMSPEED / 2))
		tile += 8;

	//these really need to be defined...
	*(uint16_t*)0xE108000 = SPRITEA_BUILD(tile + 256, 1, 0);
	*(uint32_t*)0xE108200 = SPRITEB_BUILD((playerX * 16) + 8, (playerY * 16) - 24, 0, 1, flip, 0, 1, 1);
}

void drawTile(int i, int j, int tile)
{
	if (tile == WALL && j < BOUNDS)
	{
		if (map[(j * BOUNDS) + BOUNDS + i] != WALL)
			tile = WALLALT;
	}
	int pos = (j * 128) + (i * 2);
	MAP1[pos] = 4 + (tile * 4) + 0;
	MAP1[pos + 1] = 4 + (tile * 4) + 1;
	MAP1[pos + 64] = 4 + (tile * 4) + 2;
	MAP1[pos + 65] = 4 + (tile * 4) + 3;
}

void draw()
{
	char* here = map;
	//REG_INTRMODE |= 0x80;
	REG_SCROLLX1 = -8;
	REG_SCROLLY1 = 8;
	for (int j = 0; j < BOUNDS; j++)
		for (int i = 0; i < BOUNDS; i++)
			drawTile(i, j, *here++);
	drawPlayer();
}

void move(char byX, char byY)
{
	//char *under = &map[(playerY * BOUNDS) + playerX];
	char *here = &map[((playerY + byY) * BOUNDS) + playerX + byX];
	lastDir = 0;
	if (byX == -1) lastDir = 1;
	else if (byY == 1) lastDir = 2;
	else if (byY == -1) lastDir = 3;
	if (*here == WALL)
		return;
	else if (*here & BOX)
	{
		//We might be able to push this!
		char *further = &map[((playerY + byY + byY) * BOUNDS) + playerX + byX + byX];
		if ((*further != WALL) && !(*further & BOX))
		{
			*further |= BOX;
			//*here &= ~BOX;
			if (*here == BOX) *here = SPACE;
			else if (*here == BOXINGOAL) *here = GOAL;
			drawTile(playerX + byX, playerY + byY, *here);
			playerX += byX;
			playerY += byY;
			drawTile(playerX + byX, playerY + byY, *further);
			drawPlayer();
		}
	}
	else
	{
		playerX += byX;
		playerY += byY;
		drawPlayer();
	}
}

void loadFromCode(const char* source)
{
	char ch = *source;
	int wasDigit = 0;
	int row = 0, col = 0;
	int len = 1;
	int type = -1;
	for (int i = 0; i < BOUNDS * BOUNDS; i++)
		map[i] = EXTERIOR;
	while ((ch = *source++))
	{
		//REG_DEBUGOUT = ch;
		type = -1;
		switch (ch)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				if (!wasDigit)
					len = 0;
				len = (len * 10) + (ch - '0');
				wasDigit = 1;
				break;
			case '|':
				wasDigit = 0;
				len = 1;
				row++;
				col = 0;
				break;
			case ' ':
			case '_':
			case '-':
				type = SPACE;
				break;
			case '$':
				type = BOX;
				break;
			case '.':
				type = GOAL;
				break;
			case '*':
				type = BOXINGOAL;
				break;
			case '#':
				type = WALL;
				break;
			case 'x':
				type = EXTERIOR;
				break;
			case '@':
			case '+':
				type = (ch == '@') ? SPACE : GOAL;
				playerX = col;
				playerY = row;
				break;
		}
		if (type != -1)
		{
			while (len > 0)
			{
				map[(row * BOUNDS) + col] = type;
				col++;
				len--;
			}
			len = 1;
			wasDigit = 0;
		}
	}
	REG_DEBUGOUT = '\n';
}

void load(const char* source)
{
	char* here = map;
	int k = 0;
	for (int j = 0; j < 16; j++)
	{
		for (int i = 0; i < 19; i++)
		{
			switch (source[k++])
			{
				case ' ': *here++ = SPACE; break;
				case '$': *here++ = BOX; break;
				case '.': *here++ = GOAL; break;
				case '*': *here++ = BOXINGOAL;  break;
				case '#': *here++ = WALL; break;
				case '@':
					*here++ = SPACE;
					playerX = i;
					playerY = j;
					break;
				case 0: return;
			}
		}
		here++;
	}
}

int checkWin()
{
	for (int i = 0; i < BOUNDS * BOUNDS; i++)
	{
		if (map[i] == GOAL)
		{
			return 0;
		}
	}
	return 1;
}

void nextLevel()
{
	levelNum++;
	if (levelNum > 0)
		DRAW->FadeToWhite();
	loadFromCode(levels[levelNum]);
	lastDir = 2;
	draw();
	DRAW->FadeFromWhite();
}

static int musicTimer[4];
static char* musicCursor[4];
static int musicChannel[4];
static int musicLastNote[4];
static int musicNumTracks;
extern const char* const musicTracks[];
extern const char musicSettings[];

void music()
{
	if (musicNumTracks == -1)
	{
		musicNumTracks = musicSettings[0];
		for (int i = 0; i < musicNumTracks; i++)
		{
			musicTimer[i] = 0;
			musicCursor[i] = (char*)musicTracks[i];
			musicChannel[i] = musicSettings[1 + (i * 2)];
			MIDI_PROGRAM(musicChannel[i], musicSettings[2 + (i * 2)]);
		}
	}
	for (int i = 0; i < musicNumTracks; i++)
	{
		if (musicTimer[i] == 0)
		{
			if (musicLastNote[i] > 0)
				MIDI_KEYOFF(musicChannel[i], musicLastNote[i], 80);
			char newNote = *musicCursor[i]++;
			if (newNote == 1)
			{	//repeat
				musicCursor[i] = (char*)musicTracks[i];
				continue;
			}
			char length = *musicCursor[i]++;
			if ((length & 0x80) == 0)
			{	// not tied?
				if (musicLastNote[i] > 0)
					MIDI_KEYOFF(musicChannel[i], musicLastNote[i], 80);
				if (newNote > 0)
					MIDI_KEYON(musicChannel[i], newNote, 80);
			}
			length &= ~0x80;
			musicLastNote[i] = newNote;
			musicTimer[i] = (128 / length) / 2;
		}
		else
			musicTimer[i]--;
	}
}

void WaitForKey()
{
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

int main(void)
{
	interface = (IBios*)(0x01000000);

	DRAW->DisplayPicture((TImageFile*)&titlePic);
	DRAW->FadeFromBlack();
	WaitForKey();
	DRAW->FadeToWhite();

	MISC->SetTextMode(SMODE_TILE | SMODE_SPRITES);
	MISC->DmaCopy((int8_t*)0x0E080000, (int8_t*)&tilesTiles, 1024, DMA_INT);
	MISC->DmaCopy((int8_t*)0x0E082000, (int8_t*)&playerTiles, 1024, DMA_INT);
	MISC->DmaCopy(PALETTE, (int16_t*)&tilesPal, 16, DMA_INT);
	MISC->DmaCopy(PALETTE + 32, (int16_t*)&playerPal, 16, DMA_INT);
	MISC->DmaClear(MAP1, 0, WIDTH * HEIGHT, 2);
	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);
	REG_SCREENFADE = 31;
	REG_MAPSET1 = 0x80;
	interface->VBlank = music;
	REG_INTRMODE = 0;
	musicNumTracks = -1;

	levelNum = -1;
	nextLevel();

	int in;
	for(;;)
	{

		drawPlayer(lastDir);
		for (int delay = 0; delay < 8; delay++)
		{
			vbl();
			in = REG_KEYIN;
		}
		//music();
		//if (REG_TICKCOUNT % 8 < 7)
		//	continue;
		switch (in)
		{
			case KEY_LEFT: move(-1, 0); break;
			case KEY_RIGHT: move(1, 0); break;
			case KEY_UP: move(0, -1); break;
			case KEY_DOWN: move(0, 1); break;
			case 0x13: nextLevel(); break;
		}
		if (checkWin())
			nextLevel();
	}
}
