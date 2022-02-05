#include "../ass.h"
IBios* interface;

extern const TImageFile titlePic;
extern const uint16_t tilesTiles[], tilesPal[];
extern const uint16_t playerTiles[], playerPal[];
extern const uint16_t backTiles[], backPals[];
extern const uint8_t diskettePic[];
extern const uint16_t diskettePal[];
extern const uint16_t hdma1[];
extern const char * const levels[];

extern const unsigned short imfData1[], imfData2[], imfData3[], imfData4[], imfData5[];

char *levelPack;
char *thisLevel;

#define WIDTH 40
#define HEIGHT 30

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0

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
	(((vp) & 0x7FF) << 12) |					\
	(((hp) & 0x7FF) << 0)						\
)

#define SPACE 0
#define BOX 1
#define GOAL 2
#define BOXINGOAL 3
#define WALL 4
#define EXTERIOR 6
#define BOXTOP 7
#define SPACEALT 8
#define WALLTOP 11
#define WALLALT 12

#define BOUNDS 20
#define ANIMSPEED 32

int levelNum = 0;
int moves, seconds, minutes;
long long lastTimeT;

char map[BOUNDS*BOUNDS] = {0};
int playerX = 0, playerY = 0;
int lastDir;

void PlaySound(int);

void loadBackground()
{
	int x = levelNum;
	MISC->DmaCopy(PALETTE + 20, (int16_t*)&backPals + 4 + ((x % 6) * 3), 3, DMA_INT);
	x++;
	MISC->DmaCopy(TILESET + 0x1400, (int8_t*)&backTiles + (512 * (x % 7)), 128, DMA_INT);
}

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

	OBJECTS_A[0] = OBJECTA_BUILD(tile + 256, 0, 1, 0);
	OBJECTS_B[0] = OBJECTB_BUILD((playerX * 16) + 8, (playerY * 16) - 24, 0, 1, flip, 0, 1, 3);
}

void drawTile(int i, int j, int tile)
{
	if (tile == EXTERIOR)
	{
		int pos = (j * 128) + (i * 2);
		int o = 0;
		if (i % 2 == 1) o += 2;
		if (j % 2 == 1) o += 8;
		MAP1[pos] = (160 + 0 + o) | 0x1000;
		MAP1[pos + 1] = (160 + 1 + o) | 0x1000;
		MAP1[pos + 64] = (160 + 4 + o) | 0x1000;
		MAP1[pos + 65] = (160 + 5 + o) | 0x1000;
		MAP2[pos - 64] = 0;
		MAP2[pos - 63] = 0;
		return;
	}

	int tileUp = map[(j * BOUNDS) - BOUNDS + i];
	int tileDown = map[(j * BOUNDS) + BOUNDS + i];
	if (tile == WALL && j < BOUNDS)
	{
		if (tileDown != WALL)
			tile = WALLALT;
	}
	else if (tile == SPACE && j < BOUNDS)
	{
		if (tileUp == WALL)
			tile = SPACEALT;
	}
	int pos = (j * 128) + (i * 2);
	MAP1[pos] = 4 + (tile * 4) + 0;
	MAP1[pos + 1] = 4 + (tile * 4) + 1;
	MAP1[pos + 64] = 4 + (tile * 4) + 2;
	MAP1[pos + 65] = 4 + (tile * 4) + 3;
	if (tile == BOX || tile == BOXINGOAL)
	{
		MAP2[pos - 64] = 4 + (BOXTOP * 4) + 2;
		MAP2[pos - 63] = 4 + (BOXTOP * 4) + 3;
	}
	else if ((tile == WALL || tile == WALLALT) && tileUp != WALL)
	{
		MAP2[pos - 64] = 4 + (WALLTOP * 4) + 2;
		MAP2[pos - 63] = 4 + (WALLTOP * 4) + 3;
	}
	else
	{
		MAP2[pos - 64] = 0;
		MAP2[pos - 63] = 0;
	}
}

void draw()
{
	char* here = map;
	//intoff();
	REG_SCROLLX1 = REG_SCROLLX2 = -8;
	REG_SCROLLY1 = REG_SCROLLY2 = 8;
	for (int j = 0; j < BOUNDS; j++)
	{
		for (int i = 0; i < BOUNDS; i++)
			drawTile(i, j, *here++);

		drawTile(31, j, EXTERIOR);
	}
	drawPlayer();
}

void drawStatus()
{
	char buffer[256];
	TEXT->Format(buffer, "LEVEL %d   TIME %02d:%02d   MOVES %d   ", levelNum + 1, minutes, seconds, moves);
	char *c = buffer;
	int pos = (29 * 64) + 3;
	while (*c)
	{
		MAP3[pos++] = (*c - ' ' + 64) | 0x1000;
		c++;
	}
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
			PlaySound(2);
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
		PlaySound(3);
		playerX += byX;
		playerY += byY;
		moves++;
		drawStatus();
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
	//REG_DEBUGOUT = '\n';
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
	if (*thisLevel == 0)
	{
		//Reached a blank level? Must be the end. Cool beans.
		//TODO: make this a nice bitmap image instead.
		MISC->SetBitmapMode256(SMODE_240);
		MISC->DmaClear((void*)MEM_VRAM, 0, 640*240/4, DMA_INT);
		DRAW->ResetPalette();
		DRAW->DrawString(
			"Congratulations, you beat every single level in this pack.\n"
			"Reminder to self: make a nice win screen.",
			8, 8, 15
		);
		//remove the object
		OBJECTS_B[0] = OBJECTB_BUILD(-16, -24, 0, 1, 0, 0, 1, 3);
		DRAW->FadeFromWhite();
		while(1);
	}

	levelNum++;
	if (levelNum > 0)
		DRAW->FadeToWhite();
	loadFromCode(thisLevel);
	while(*thisLevel++) ;
	lastDir = 2;
	moves = seconds = minutes = 0;
	draw();
	drawStatus();
	loadBackground();
	DRAW->FadeFromWhite();
	lastTimeT = REG_TIMET;
}

extern const unsigned char jingleSound[];
extern const unsigned char slideSound[];
extern const unsigned char stepSound[];
const unsigned char * const sounds[] = { 0, jingleSound, slideSound, stepSound };

extern int IMF_LoadSong(const unsigned short *sauce, int loop);
extern void IMF_Play();

void PlaySound(int id)
{
	int soundData = (int)sounds[id];
	if (id == 1)
	{
		PCMOFFSET[1] = soundData + 4;
		PCMLENGTH[1] = *(unsigned int*)soundData;
	}
	else
	{
		PCMOFFSET[0] = soundData + 4;
		PCMLENGTH[0] = *(unsigned int*)soundData;
	}
}

void WaitForKey()
{
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0 && REG_JOYPAD == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

int getc()
{
	unsigned short key = 0;
	while (1)
	{
		key = REG_KEYIN;
		if (key & 0xFF) break;
		vbl();
	}
	while (REG_KEYIN != 0) { vbl(); }
	return key;
}

void RotateTheFloppy()
{
	static int frame = 0, timer = 0;
	if (timer == 0)
	{
		REG_BLITSOURCE = (uint32_t)&diskettePic[frame * 0x200];
		REG_BLITTARGET = MEM_VRAM + (96 * 320) + 24;
		REG_BLITLENGTH = 0x200;
		REG_BLITCONTROL = BLIT_COPY | BLIT_BYTE | BLIT_STRIDESKIP | BLIT_SOURCESTRIDE(16) | BLIT_TARGETSTRIDE(320);

		frame++;
		if (frame == 8) frame = 0;
		timer = 8;
	}
	else
		timer--;
}

void CheckForDisk()
{
	const char* msg =
		"A diskette has been inserted with a level pack file on it.\n"
		"Would you like to play that instead of the built-in pack?\n\n[Y/n]";
	FILEINFO nfo;
	DIR dir;
	FILE file;
	int ret = DISK->FindFirst(&dir, &nfo, "0:/", "sokoban.txt");
	if (ret != 0) return;
	if (nfo.fname[0] == 0) return;

	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	MISC->SetBitmapMode16(SMODE_240);
	MISC->DmaClear((void*)MEM_VRAM, 0, 640*240/4, DMA_INT);

	MISC->DmaCopy(PALETTE, (int16_t*)&diskettePal, 16, DMA_INT);
	interface->VBlank = RotateTheFloppy;
	interface->DrawCharFont = (char*)0x0E062200;
	interface->DrawCharHeight = 0x0B10;

	DRAW->DrawString(msg, 121, 97, 9);
	DRAW->DrawString(msg, 120, 96, 5);
	DRAW->FadeFromWhite();
	while (1)
	{
		int key = getc();
		if (key == 0x15 || key == 0x1C) //y or enter
			break;
		else if (key == 0x31) //n
		{
			DRAW->FadeToWhite();
			REG_HDMACONTROL[0] = 0;
			return;
		}
	}

	levelPack = (char*)malloc(nfo.fsize);
	ret = DISK->OpenFile(&file, "0:/sokoban.txt", FA_READ);
	ret = DISK->ReadFile(&file, (void*)levelPack, nfo.fsize);
	ret = DISK->CloseFile(&file);
	char* c = levelPack;
	while (*c)
	{
		if (*c == '\n')
			*c = 0;
		c++;
	}
	DRAW->FadeToWhite();
	REG_HDMACONTROL[0] = 0;
}

int main(void)
{
	REG_SCREENFADE = 31;

	MISC->DmaClear(TILESET, 0, 0x4000, DMA_INT);
	MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);

	DRAW->DisplayPicture((TImageFile*)&titlePic);
	DRAW->FadeFromBlack();
	WaitForKey();
	DRAW->FadeToWhite();

	//REG_HDMASOURCE[0] = (int32_t)hdma1;
	//REG_HDMATARGET[0] = (int32_t)PALETTE;
	//REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);
	REG_SCREENFADE = 31;
	REG_MAPSET = 0x70;

	levelPack = (char*)levels[0];
	CheckForDisk();
	//levelPack = (char**)diskLevels;
	thisLevel = levelPack;

	MISC->SetTextMode(SMODE_TILE);
	MISC->DmaCopy(TILESET, (int8_t*)&tilesTiles, 0x480, DMA_INT); //TODO: check
	MISC->DmaCopy(TILESET + 0x2000, (int8_t*)&playerTiles, 1024, DMA_INT);
	MISC->DmaCopy(PALETTE, (int16_t*)&tilesPal, 16, DMA_SHORT);
	MISC->DmaCopy(PALETTE + 16, (int16_t*)&backPals, 4, DMA_SHORT);
	MISC->DmaCopy(PALETTE + 256, (int16_t*)&playerPal, 16, DMA_SHORT);
	MISC->DmaClear(MAP1, 0, WIDTH * HEIGHT, 2);
	MISC->DmaClear(MAP2, 0, WIDTH * HEIGHT, 2);
	MISC->DmaClear(MAP3, 0, WIDTH * HEIGHT, 2);

	interface->VBlank = IMF_Play;
	inton();

	levelNum = -1;
	IMF_LoadSong(imfData2, 1);
	PlaySound(1);
	nextLevel();

	int in;
	for(;;)
	{
		drawPlayer(lastDir);
		for (int delay = 0; delay < 12; delay++)
		{
			vbl();
			in = REG_KEYIN;
			if (REG_JOYPAD & 1) in = KEY_UP;
			else if (REG_JOYPAD & 2) in = KEY_RIGHT;
			else if (REG_JOYPAD & 4) in = KEY_DOWN;
			else if (REG_JOYPAD & 8) in = KEY_LEFT;
		}
		if (REG_TIMET > lastTimeT)
		{
			seconds += REG_TIMET - lastTimeT;
			while (seconds >= 60)
			{
				minutes++;
				seconds -= 60;
			}
			drawStatus();
			lastTimeT = REG_TIMET;
		}
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
