#include "../ass.h"
#include "funcs.h"

#define SCOPE 256
#define MAXWARP 10
#define MINWARP 0
#define MINSTARS 10
#define MAXSTARS 64
#define WARPFACTOR 10
#define DEF_DENSITY 25
#define RAND(x) ((rand() % (x))+1)
#define ZRAND(x) (rand() % (x))
#define MINTIMERSPEED 50

#define SCREENWIDTH 640
#define SCREENHEIGHT 480
#define HALFWIDTH 320
#define HALFHEIGHT 240
#define WARPSPEED MINWARP
#define DENSITY MAXSTARS
#define WARP (WARPSPEED * WARPFACTOR + WARPFACTOR)

typedef struct
{
	int x, y, z;
	int sx, sy;
} star;

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

static inline int max(int a, int b)
{
	return (a < b) ? b : a;
}

static inline void CreateStar(star *s)
{
	s->x = ZRAND(SCREENWIDTH) - HALFWIDTH;
	s->y = ZRAND(SCREENWIDTH) - HALFHEIGHT;
	s->z = SCOPE * WARPFACTOR;
}

static inline void ProjectStar(star *s)
{
	s->sx = ((s->x * (SCOPE * WARPFACTOR)) / s->z) + HALFWIDTH;
	s->sy = ((s->y * SCOPE * WARPFACTOR) / s->z) + HALFHEIGHT;
}

void Starfield()
{
	star stars[MAXSTARS];

	Fade(false, false);
	int8_t prevScreenMode = REG_SCREENMODE;
	uint8_t* prevScreen = malloc(SCREENWIDTH * SCREENHEIGHT);
	uint16_t* prevObjs = malloc(0x1000);
	uint16_t prevBlack = PALETTE[254];
	uint16_t prevWhite = PALETTE[255];
	DmaCopy(prevObjs, OBJECTS_A, 0x1000, DMA_INT);
	DmaCopy(prevScreen, BITMAP, SCREENWIDTH * SCREENHEIGHT, DMA_BYTE);
	REG_SCREENMODE = SMODE_BMP256;
	DmaClear(BITMAP, 0xFEFE, SCREENWIDTH * SCREENHEIGHT, DMA_BYTE);
	DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);
	PALETTE[254] = 0x0000;
	PALETTE[255] = 0x7FFF;

	for (int i = 0; i < DENSITY; i++)
		CreateStar(&stars[i]);

	while (!INP_KEYIN)
	{
		for (int i = 0; i < DENSITY; i++)
		{
			BITMAP[(stars[i].sy * SCREENWIDTH) + stars[i].sx] = 254;

			stars[i].z = max(0, stars[i].z - WARP);
			if (!stars[i].z)
				CreateStar(&stars[i]);

			ProjectStar(&stars[i]);

			if ((stars[i].sx < 0 || stars[i].sy < 0) || (stars[i].sx > SCREENWIDTH || stars[i].sy > SCREENHEIGHT))
			{
				CreateStar(&stars[i]);
				ProjectStar(&stars[i]);
			}

			BITMAP[(stars[i].sy * SCREENWIDTH) + stars[i].sx] = 255;
		}

		REG_SCREENFADE = 0;

		vbl();
	}

	Fade(false, false);
	DmaCopy(BITMAP, prevScreen, SCREENWIDTH * SCREENHEIGHT, DMA_BYTE);
	DmaCopy(OBJECTS_A, prevObjs, 0x1000, DMA_INT);
	free(prevScreen);
	free(prevObjs);
	REG_SCREENMODE = prevScreenMode;
	PALETTE[254] = prevBlack;
	PALETTE[255] = prevWhite;
	Fade(true, false);
}
