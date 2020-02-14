#include "../ass.h"

#include "game.h"

IBios* interface;

extern const uint16_t tilesTiles[], farahTiles[], logoTiles[];
extern const uint16_t tilesPal[], farahPal[];
extern const uint16_t backgroundMap[];

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

static const uint16_t spritesA[] = {
	SPRITEA_BUILD(128, 1, 4),
	SPRITEA_BUILD(128 + 8, 1, 4),
	SPRITEA_BUILD(128 + 12, 1, 4),
	SPRITEA_BUILD(128 + 16, 1, 4),
	SPRITEA_BUILD(128 + 32, 1, 4),
	SPRITEA_BUILD(128 + 48, 1, 4),
	SPRITEA_BUILD(128 + 56, 1, 4),
	SPRITEA_BUILD(384, 1, 1),
	SPRITEA_BUILD(384 + 16, 1, 1),
	SPRITEA_BUILD(384 + 32, 1, 1),
	SPRITEA_BUILD(384 + 48, 1, 1),
	SPRITEA_BUILD(384 + 56, 1, 1),
	SPRITEA_BUILD(384 + 64, 1, 1),
	SPRITEA_BUILD(384 + 72, 1, 1),
	SPRITEA_BUILD(384 + 80, 1, 1),
	0,
};
static const uint32_t spritesB[] = {
	SPRITEB_BUILD(228, 24, 1, 0, 0, 0, 1, 0),
	SPRITEB_BUILD(260, 24, 0, 0, 0, 0, 1, 0),
	SPRITEB_BUILD(262, 72, 0, 0, 0, 0, 1, 0),
	SPRITEB_BUILD(224, 40, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(256, 40, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(230, 72, 1, 0, 0, 0, 1, 0),
	SPRITEB_BUILD(224, 48, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(168, 176, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(200, 176, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(232, 176, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(264, 176, 0, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(168, 208, 1, 0, 0, 0, 1, 0),
	SPRITEB_BUILD(200, 208, 1, 0, 0, 0, 1, 0),
	SPRITEB_BUILD(232, 208, 1, 0, 0, 0, 1, 0),
	SPRITEB_BUILD(264, 208, 1, 0, 0, 0, 0, 0),
	0,
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
	MISC->DmaCopy(TILESET, (int8_t*)&tilesTiles, 1024, DMA_INT);
	MISC->DmaCopy(TILESET + 0x1000, (int8_t*)&farahTiles, 1024, DMA_INT);
	MISC->DmaCopy(TILESET + 0x3000, (int8_t*)&logoTiles, 1024, DMA_INT);
	MISC->DmaCopy(PALETTE, (int16_t*)&tilesPal, 64, DMA_INT);
	MISC->DmaCopy(PALETTE + 64, (int16_t*)&farahPal, 64, DMA_INT);
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
		uint16_t* sa = (uint16_t*)spritesA;
		uint32_t* sb = (uint32_t*)spritesB;
		int i = 0;
		while (*sa)
		{
			SPRITES_A[i] = *sa++;
			SPRITES_B[i] = *sb++;
			i++;
		}
	}

	DRAW->FadeFromBlack();

	game *game = init_game();
	do
	{
		run_game(game);
	} while (game_over(game) == NEW_GAME);
	free_game(game);
}

