#include "../ass.h"
#define sprintf(b,f,rest...) TEXT->Format(b,f, ## rest)
IBios* interface;

#define __PLAYERS G(vicViper) G(lordBritish)
#define G(x) extern const unsigned short x ## Tiles[], x ## Pal[];
__PLAYERS
#undef G
const unsigned short* const playerTiles[] = {
#define G(x) x ## Tiles,
__PLAYERS
#undef G
};
const unsigned short* const playerPal[] = {
#define G(x) x ## Pal,
__PLAYERS
#undef G
};
#undef __PLAYERS

extern const uint16_t fontTiles[], fontPal[];
extern const uint16_t starfieldTiles[], starfieldPal[], starfieldMap[];

char debugBuffer[256];
extern int scroll;

#include "actor.h"

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

void print(char* str, int x, int y, int color)
{
	unsigned short *t = &MAP2[(y * 64) + x];
	color *= 64;
	char *b = str;
	while (*b)
	{
		*t++ = ((*b - 32) + color) | 0xF000;
		b++;
	}
}

int bitTest(int* set, int bit)
{
	return set[bit / 32] & (1 << (bit % 32));
}

int bitSet(int* set, int bit)
{
	int old = bitTest(set, bit);
	set[bit / 32] |= (1 << (bit % 32));
	return old;
}

int bitClear(int* set, int bit)
{
	int old = bitTest(set, bit);
	set[bit / 32] &= ~(1 << (bit % 32));
	return old;
}

int main(void)
{
	REG_SCREENMODE = SMODE_TILE;
	REG_SCREENFADE = 0;

	intoff();

	REG_MAPSET = 0x30;
	MISC->DmaClear(MAP2, 0, 64 * 32, DMA_SHORT);
	for (int i = 0; i < 256/32; i++)
		objectsUsed[i] = 0;

	MISC->DmaClear(entities, 0, sizeof(tEntity) * 64, DMA_BYTE);

	MISC->DmaCopy(TILESET, (int8_t*)&fontTiles, 1024, DMA_INT);
	MISC->DmaCopy(PALETTE + 240, (int8_t*)&fontPal, 16, DMA_SHORT);

	MISC->DmaCopy(TILESET + (64 * 32), (int8_t*)&starfieldTiles, 256, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&starfieldPal, 16, DMA_SHORT);
	MISC->DmaCopy(MAP1, (int8_t*)&starfieldMap, 32 * 64, DMA_SHORT);

	Spawn(1, 64, 128);
	int e = Spawn(3, 0, 0);
	entities[e].extra[1] = 1000;
	//e = Spawn(1, 48, 64);
	//entities[e].obj = 4;
	//entities[e].pal = 3;

	print("SALANYANDER INDEV", 0, 0, 0);

	for(;;)
	{
		sprintf(debugBuffer, "SCROLL %d  REG_KEYIN $%02X ", scroll, REG_KEYIN);
		print(debugBuffer, 0, 1, 0);
		sprintf(debugBuffer, "OBJUSE %04X%04X%04X%04X", objectsUsed[3], objectsUsed[2], objectsUsed[1], objectsUsed[0]);
		print(debugBuffer, 0, 2, 0);

		Think();
		Draw();
		scroll++;
		REG_SCROLLX1 = scroll >> 1;

		vbl();
	}
}
