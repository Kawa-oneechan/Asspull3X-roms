#include "../ass.h"
#include "../ass-midi.h"

#include "game.h"

IBios* interface;

extern const TPicFile title;
extern const uint16_t tilesTiles[], farahTiles[];
extern const uint16_t logoTiles[], logo_kotrisTiles[], logobigTiles[], logobig_kotrisTiles[], press_startTiles[];
extern const uint16_t tilesPal[];
extern const uint16_t backgroundMap[];

extern const uint16_t imfData1[];

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

static const uint16_t logoA[] = {
	OBJECTA_BUILD(  0, 0, 1, 1),
	OBJECTA_BUILD( 16, 0, 1, 1),
	OBJECTA_BUILD( 32, 0, 1, 1),
	OBJECTA_BUILD( 48, 0, 1, 1),
	OBJECTA_BUILD( 64, 0, 1, 1),
	OBJECTA_BUILD( 80, 0, 1, 1),
	OBJECTA_BUILD( 96, 0, 1, 1),
	OBJECTA_BUILD(112, 0, 1, 1),
	0,
};
static const uint32_t logoB[] = {
	OBJECTB_BUILD( 0,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(32,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(64,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(96,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD( 0, 32, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(32, 32, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(64, 32, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(96, 32, 1, 1, 0, 0, 1, 0),
	0,
};
static const uint16_t bigLogoA[] = {
	OBJECTA_BUILD(  0, 0, 1, 1),
	OBJECTA_BUILD( 16, 0, 1, 1),
	OBJECTA_BUILD( 32, 0, 1, 1),
	OBJECTA_BUILD( 48, 0, 1, 1),
	OBJECTA_BUILD( 64, 0, 1, 1),
	OBJECTA_BUILD( 80, 0, 1, 1),
	OBJECTA_BUILD( 96, 0, 1, 1),
	OBJECTA_BUILD(112, 0, 1, 1),
	OBJECTA_BUILD(128, 0, 1, 1),
	OBJECTA_BUILD(144, 0, 1, 1),
	OBJECTA_BUILD(160, 0, 1, 1),
	OBJECTA_BUILD(176, 0, 1, 1),
	0,
};
static const uint32_t bigLogoB[] = {
	OBJECTB_BUILD(  0,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD( 32,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD( 64,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD( 96,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(128,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(160,  0, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(  0, 32, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD( 32, 32, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD( 64, 32, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD( 96, 32, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(128, 32, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(160, 32, 1, 1, 0, 0, 1, 0),
	0,
};
static const uint16_t pressStartA[] = {
	OBJECTA_BUILD(  0, 0, 1, 1),
	OBJECTA_BUILD(  8, 0, 1, 1),
};
static const uint32_t pressStartB[] = {
	OBJECTB_BUILD(  0,  0, 1, 0, 0, 0, 1, 0),
	OBJECTB_BUILD( 32,  0, 1, 0, 0, 0, 1, 0),
};
static const uint16_t noControllerA[] = {
	OBJECTA_BUILD( 24, 0, 1, 1),
	OBJECTA_BUILD( 32, 0, 1, 1),
	OBJECTA_BUILD( 40, 0, 1, 1),
};
static const uint32_t noControllerB[] = {
	OBJECTB_BUILD(  0,  0, 1, 0, 0, 0, 1, 0),
	OBJECTB_BUILD( 32,  0, 1, 0, 0, 0, 1, 0),
	OBJECTB_BUILD( 64,  0, 1, 0, 0, 0, 1, 0),
};

const uint8_t * const sounds[] = { 0 };

extern int IMF_LoadSong(const uint16_t *sauce, bool loop);
extern void IMF_Install(void(*next)(void));

void PlaySound(int id)
{
	int soundData = (int)sounds[id];
	REG_PCM1OFFSET = soundData + 4;
	REG_PCM1LENGTH = *(uint32_t*)soundData;
}

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

void WaitForKey()
{
	while (INP_KEYIN == 0 && INP_JOYPAD1 == 0) { vbl(); }
}

void DrawObject(const uint16_t* a, const uint32_t* b, int obj, int tile, int x, int y)
{
	uint16_t* sa = (uint16_t*)a;
	uint32_t* sb = (uint32_t*)b;
	while (*sa)
	{
		uint16_t na = *sa++;
		uint32_t nb = *sb++;
		OBJECTS_A[obj] = (na & ~0x1FF) | ((na & 0x1FF) + tile);
		OBJECTS_B[obj] = (nb & ~0x3FFFFF) | (((nb & 0x7FF) + x) & 0x7FF) | (((((nb >> 12) & 0x3FF) + y) & 0x3FF) << 12);
		obj++;
	}
}

void LoadFarah(int face)
{
	MISC->DmaCopy(TILESET + 0xC00, (int8_t*)&farahTiles + (face * 2240), 560, DMA_INT);
}

//And now, an absolute freakin' hack to get double-speed music.
//Problem: can't use AudioT sound effects any more.
uint16_t imfCycles = 1;
void(*imfPlusPlay)(void);
void(*imfPlusNext)(void);
void IMFPlus_Play()
{
	if (imfCycles == 1)
		imfPlusPlay();
	else
		for (int i = 0; i < imfCycles; i++)
			imfPlusPlay();

	if (imfPlusNext)
		imfPlusNext();
}
void IMFPlus_Install(void(*next)(void))
{
	if (next == NULL)
		next = interface->vBlank;

	//Install original handler to grab IMF_Play
	IMF_Install(NULL);
	imfPlusPlay = interface->vBlank;

	//Replace with our own.
	imfPlusNext = next;
	interface->vBlank = IMFPlus_Play;
}

int main(void)
{
	REG_SCREENFADE = 31;
	bool kotris = (REG_TIMET % 24 >= 12);

	DRAW->DisplayPicture(&title);
	if (kotris)
		MISC->DmaCopy(TILESET, (int8_t*)&logobig_kotrisTiles, 1536, DMA_INT);
	else
		MISC->DmaCopy(TILESET, (int8_t*)&logobigTiles, 1536, DMA_INT);
	MISC->DmaCopy(TILESET + 6144, (int8_t*)&press_startTiles, 384, DMA_INT);
	MISC->DmaCopy(PALETTE + 256, (int16_t*)&tilesPal, 32, DMA_SHORT);

	IMFPlus_Install(NULL);

	DRAW->Fade(true, false);
	//WaitForKey();
	{
		int i = -64 - 9;
		while (!INP_JOYPAD1)
		{
			DrawObject(bigLogoA, bigLogoB, 16, 0, 69, i);
			vbl();
			if (i < 9) i += 2;

			if (INP_JOYSTATES & 3)
				DrawObject(pressStartA, pressStartB, 1, 192, 16, 211);
			else
				DrawObject(noControllerA, noControllerB, 1, 192, 16, 211);
		}
	}
	DRAW->Fade(false, true);

//	IMF_Install(NULL);
	MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);

	REG_SCREENMODE = SMODE_TILE;
	MISC->DmaCopy(TILESET, (int8_t*)&tilesTiles, 1024 + 320, DMA_INT);
	if (kotris)
		MISC->DmaCopy(TILESET + 0x3000, (int8_t*)&logo_kotrisTiles, 1024, DMA_INT);
	else
		MISC->DmaCopy(TILESET + 0x3000, (int8_t*)&logoTiles, 1024, DMA_INT);
	LoadFarah(0);
	MISC->DmaCopy(PALETTE, (int16_t*)&tilesPal, 64, DMA_SHORT);
	MISC->DmaCopy(PALETTE + 256, (int16_t*)&tilesPal, 32, DMA_SHORT);
	//MISC->DmaCopy(PALETTE + 256, (int16_t*)&farahPal, 16, DMA_SHORT);
	MISC->DmaClear(MAP1, 0, 64 * 64, 2);
	MISC->DmaClear(MAP2, 0, 64 * 64, 2);
	REG_MAPSET = 0x30;
	for (int r = 0; r < GRID_ROWS; r++)
		for (int c = 0; c < GRID_COLS; c++)
			MAP1[c + 4 + ((r + 4) * 64)] = 9;
	REG_SCROLLX2 = -48;
	REG_SCROLLY2 = -32;

	{
		uint16_t* dst = MAP1;
		uint16_t* src = (uint16_t*)backgroundMap;
		for (int line = 0; line < 30; line++)
		{
			for (int row = 0; row < 40; row++)
				*dst++ = *src++;
			dst += 24;
		}
	}

	DrawObject(logoA, logoB, 16, 384, 168, 176);

	IMF_LoadSong(imfData1, true);

	DRAW->Fade(true, true);

	game *game = init_game();
	do
	{
		run_game(game);
	} while (game_over(game) == NEW_GAME);
}

