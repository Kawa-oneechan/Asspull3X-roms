#include "../ass.h"
IBios* interface;

unsigned char rndtable[256] = {
	0,   8, 109, 220, 222, 241, 149, 107,  75, 248, 254, 140,  16,  66 ,
	74,  21, 211,  47,  80, 242, 154,  27, 205, 128, 161,  89,  77,  36 ,
	95, 110,  85,  48, 212, 140, 211, 249,  22,  79, 200,  50,  28, 188 ,
	52, 140, 202, 120,  68, 145,  62,  70, 184, 190,  91, 197, 152, 224 ,
	149, 104,  25, 178, 252, 182, 202, 182, 141, 197,   4,  81, 181, 242 ,
	145,  42,  39, 227, 156, 198, 225, 193, 219,  93, 122, 175, 249,   0 ,
	175, 143,  70, 239,  46, 246, 163,  53, 163, 109, 168, 135,   2, 235 ,
	25,  92,  20, 145, 138,  77,  69, 166,  78, 176, 173, 212, 166, 113 ,
	94, 161,  41,  50, 239,  49, 111, 164,  70,  60,   2,  37, 171,  75 ,
	136, 156,  11,  56,  42, 146, 138, 229,  73, 146,  77,  61,  98, 196 ,
	135, 106,  63, 197, 195,  86,  96, 203, 113, 101, 170, 247, 181, 113 ,
	80, 250, 108,   7, 255, 237, 129, 226,  79, 107, 112, 166, 103, 241 ,
	24, 223, 239, 120, 198,  58,  60,  82, 128,   3, 184,  66, 143, 224 ,
	145, 224,  81, 206, 163,  45,  63,  90, 168, 114,  59,  33, 159,  95 ,
	28, 139, 123,  98, 125, 196,  15,  70, 194, 253,  54,  14, 109, 226 ,
	71,  17, 161,  93, 186,  87, 244, 138,  20,  52, 123, 251,  26,  36 ,
	17,  46,  52, 231, 232,  76,  31, 221,  84,  37, 216, 165, 212, 106 ,
	197, 242,  98,  43,  39, 175, 254, 145, 190,  84, 118, 222, 187, 136 ,
	120, 163, 236, 249
};

int	rndindex = 0;

uint32_t rand()
{
	rndindex = (rndindex + 1) & 0xFF;
	return rndtable[rndindex];
}

#define DECAY 10
#define MARGIN 32

int main(void)
{
	MISC->SetBitmapMode256(SMODE_320 | SMODE_240);
	for (int i = 0; i < 32; i++)
		PALETTE[i] = i;
	for (int i = 0; i < 32; i++)
		PALETTE[i + 32] = 0x001F | (i << 5);
	for (int i = 0; i < 32; i++)
		PALETTE[i + 64] = 0x07FF | (i << 10);

	MISC->DmaClear(BITMAP, 0, 320*240, DMA_BYTE);

	int o = 238 * 320;
	for (int i = MARGIN; i < 318 - MARGIN; i += 1)
	{
		BITMAP[o + i] = 95;
		BITMAP[o + 320 + i] = 95;
	}

	int32_t offs[240];
	for (int i = 0; i < 240; i++)
	{
		offs[i] = i * 320;
	}

	while (!INP_KEYIN)
	{
		for (int c = MARGIN; c < 318 - MARGIN; c += 4)
		{
			for (int r = 239 - 64; r <= 239; r += 2)
			{
				int h = offs[r] + c;
				//int h = (r * 320) + c;
				int nh = h - 640;
				int v = BITMAP[h];
				if (v == 0 || v < DECAY || c <= 0 || c >= 320)
				{
					BITMAP[nh] = BITMAP[nh + 1] = BITMAP[nh + 320] = BITMAP[nh + 320 + 1] =
					BITMAP[nh + 2] = BITMAP[nh + 3] = BITMAP[nh + 320 + 2] = BITMAP[nh + 320 + 3] = 0;
				}
				else
				{
					nh += ((rand() % 3) << 2) - 4;
					BITMAP[nh] = BITMAP[nh + 1] = BITMAP[nh + 320] = BITMAP[nh + 320 + 1] =
					BITMAP[nh + 2] = BITMAP[nh + 3] = BITMAP[nh + 320 + 2] = BITMAP[nh + 320 + 3] =v - (rand() % DECAY);
				}
			}
		}
		//vbl();
	}
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	TEXT->ClearScreen();
	printf("Shoutouts to Fabian Sanglard.\n");
}
