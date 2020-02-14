#include "../ass.h"

#include "game.h"

IBios* interface;

extern const uint16_t tilesTiles[], tilesPal[], backgroundMap[];

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0

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
	MISC->DmaCopy(PALETTE, (int16_t*)&tilesPal, 64, DMA_INT);
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
	DRAW->FadeFromBlack();

	game *game = init_game();
	do
	{
		run_game(game);
	} while (game_over(game) == NEW_GAME);
	free_game(game);
}

