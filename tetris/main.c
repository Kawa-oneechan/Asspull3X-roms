#include "../ass.h"
#include "../ass-midi.h"

#include "game.h"

IBios* interface;

extern const TPicFile title;
extern const uint16_t tilesTiles[], farahTiles[], logoTiles[];
extern const uint16_t tilesPal[], farahPal[];
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

static const uint16_t objectsA[] = {
	OBJECTA_BUILD(128, 0, 1, 0),
	OBJECTA_BUILD(128 + 16, 0, 1, 0),
	OBJECTA_BUILD(128 + 32, 0, 1, 0),
	OBJECTA_BUILD(128 + 48, 0, 1, 0),

	OBJECTA_BUILD(128 + 64, 0, 1, 0),

	OBJECTA_BUILD(384, 0, 1, 1),
	OBJECTA_BUILD(384 + 16, 0, 1, 1),
	OBJECTA_BUILD(384 + 32, 0, 1, 1),
	OBJECTA_BUILD(384 + 48, 0, 1, 1),
	OBJECTA_BUILD(384 + 64, 0, 1, 1),
	OBJECTA_BUILD(384 + 80, 0, 1, 1),
	OBJECTA_BUILD(384 + 96, 0, 1, 1),
	OBJECTA_BUILD(384 + 112, 0, 1, 1),
	0,
};
static const uint32_t objectsB[] = {
	OBJECTB_BUILD(224, 24, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(256, 24, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(224, 56, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(256, 56, 1, 1, 0, 0, 1, 0),

	OBJECTB_BUILD(224, 48, 1, 1, 0, 0, 1, 0),

	OBJECTB_BUILD(168, 176, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(200, 176, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(232, 176, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(264, 176, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(168, 208, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(200, 208, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(232, 208, 1, 1, 0, 0, 1, 0),
	OBJECTB_BUILD(264, 208, 1, 1, 0, 0, 1, 0),
	0,
};

const uint8_t * const sounds[] = { 0 };

extern int IMF_LoadSong(const uint16_t *sauce, bool loop);
extern void IMF_Play();

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

int main(void)
{
	REG_SCREENFADE = 31;
	DRAW->DisplayPicture(&title);
	interface->vBlank = IMF_Play;
	DRAW->Fade(true, false);
	WaitForKey();
	DRAW->Fade(false, true);

	REG_SCREENMODE = SMODE_TILE;
	MISC->DmaCopy(TILESET, (int8_t*)&tilesTiles, 1024, DMA_INT);
	MISC->DmaCopy(TILESET + 0x1000, (int8_t*)&farahTiles, 1024, DMA_INT);
	MISC->DmaCopy(TILESET + 0x3000, (int8_t*)&logoTiles, 1024, DMA_INT);
	MISC->DmaCopy(PALETTE, (int16_t*)&tilesPal, 48, DMA_SHORT);
	MISC->DmaCopy(PALETTE + 256, (int16_t*)&tilesPal, 32, DMA_SHORT);
	MISC->DmaCopy(PALETTE + 256, (int16_t*)&farahPal, 16, DMA_SHORT);
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

	{
		uint16_t* sa = (uint16_t*)objectsA;
		uint32_t* sb = (uint32_t*)objectsB;
		int i = 0;
		while (*sa)
		{
			OBJECTS_A[i] = *sa++;
			OBJECTS_B[i] = *sb++;
			i++;
		}
	}

	DRAW->Fade(true, false);

	IMF_LoadSong(imfData1, true);

	game *game = init_game();
	do
	{
		run_game(game);
	} while (game_over(game) == NEW_GAME);
}

