#include "../ass.h"
#include "../ass-midi.h"

#include "game.h"

IBios* interface;

extern const TImageFile titlePic;
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
	SPRITEA_BUILD(384 + 64, 1, 1),
	SPRITEA_BUILD(384 + 80, 1, 1),
	SPRITEA_BUILD(384 + 96, 1, 1),
	SPRITEA_BUILD(384 + 112, 1, 1),
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
	SPRITEB_BUILD(264, 176, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(168, 208, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(200, 208, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(232, 208, 1, 1, 0, 0, 1, 0),
	SPRITEB_BUILD(264, 208, 1, 1, 0, 0, 1, 0),
	0,
};

static int musicTimer[4];
static char* musicCursor[4];
static int musicChannel[4];
static unsigned char musicNotes[128], musicNoteLengths[128], musicNoteChannels[128];
static char* musicRepeatFrom[4];
static char* musicRepeatTo[4];
static char musicRepeatTimes[4];
static int musicNumTracks;
extern const char* const musicTracks[];
extern const char musicSettings[];

/*
extern const char jingleSound[];
extern const char slideSound[];
extern const char stepSound[];
extern const int jingleLength;
extern const int slideLength;
extern const int stepLength;
*/
const char* currentSound;
int currentSoundLength;

void music()
{
	if (currentSound != 0)
	{
		for (int i = 0; i < 256 && currentSoundLength > 0; i++)
		{
			REG_AUDIOOUT = *currentSound;
			currentSound++;
			currentSoundLength--;
			if (currentSoundLength == 0)
				currentSound = 0;
		}
	}

	if (musicNumTracks == -1)
	{
		musicNumTracks = musicSettings[0];
		for (int i = 0; i < musicNumTracks; i++)
		{
			musicTimer[i] = 0;
			musicCursor[i] = (char*)musicTracks[i];
			musicChannel[i] = musicSettings[1 + (i * 2)];
			musicRepeatTo[i] = 0;
			musicRepeatTimes[i] = 0;
			MIDI_PROGRAM(musicChannel[i], musicSettings[2 + (i * 2)]);
		}
		for (int i = 0; i < 128; i++)
		{
			musicNotes[i] = 0xFF;
		}
	}
	for (int i = 0; i < 128; i++)
	{
		if (musicNotes[i] != 0xFF)
		{
			musicNoteLengths[i]--;
			if (musicNoteLengths[i] == 0)
			{
				MIDI_KEYOFF(musicNoteChannels[i], musicNotes[i], 80);
				musicNotes[i] = 0xFF;
			}
		}
	}
	for (int i = 0; i < musicNumTracks; i++)
	{
		if (musicTimer[i] == 0)
		{
			char newNote = *musicCursor[i]++;
			//TODO: change these non-notes to the 250-ish range.
			//TODO: add "loop N times"
			if (newNote == 1)
			{	//repeat
				musicCursor[i] = (char*)musicTracks[i];
				continue;
			}
			else if (newNote == 2)
			{	//goto
				musicCursor[i] = (char*)musicTracks[i] + (*musicCursor[i] * 2);
				continue;
			}
			else if (newNote == 3)
			{	//repeatTo
				if (musicRepeatTo[i] == musicCursor[i])
				{
					//already in a repeat
					musicRepeatTimes[i]--;
					if (musicRepeatTimes[i] == 0)
					{
						musicRepeatFrom[i] = musicRepeatTo[i] = 0;
						musicCursor[i] += 2; //skip to and count
						continue;
					}
				}
				else
				{
					musicRepeatTo[i] = musicCursor[i];
					musicRepeatFrom[i] = (char*)musicTracks[i] + (*musicCursor[i] * 2);
					musicCursor[i]++;
					musicRepeatTimes[i] = *musicCursor[i];
				}
				musicCursor[i] = musicRepeatFrom[i];
				continue;
			}
			char length = *musicCursor[i]++;
			if ((length & 0x80) == 0)
			{	// not tied?
				//if (musicLastNote[i] > 0)
				//	MIDI_KEYOFF(musicChannel[i], musicLastNote[i], 80);
				if (newNote > 0)
					MIDI_KEYON(musicChannel[i], newNote, 80);
			}
			length &= ~0x80;
			//musicLastNote[i] = newNote;
			musicTimer[i] = (128 / length);
			for (int j = 0; j < 128; j++)
			{
				if (musicNotes[j] == 0xFF)
				{
					musicNotes[j] = newNote;
					musicNoteLengths[j] = musicTimer[i];
					musicNoteChannels[j] = musicChannel[i];
					break;
				}
			}
		}
		else
			musicTimer[i]--;
	}
}

void PlaySound(int id)
{
/*
	switch (id)
	{
		case 1:
			currentSound = jingleSound;
			currentSoundLength = jingleLength;
			return;
		case 2:
			currentSound = slideSound;
			currentSoundLength = slideLength;
			return;
		case 3:
			currentSound = stepSound;
			currentSoundLength = stepLength;
			return;
		default:
			currentSound = 0;
			currentSoundLength = 0;
			return;
	}
*/
}

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

	DRAW->DisplayPicture((TImageFile*)&titlePic);
	//TEST: play music on the title screen for quick testing
	interface->VBlank = music;
	inton();
	musicNumTracks = -1;
	//-end test-
	DRAW->FadeFromBlack();
	WaitForKey();
	DRAW->FadeToWhite();

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

	interface->VBlank = music;
	inton();
	musicNumTracks = -1;

	game *game = init_game();
	do
	{
		run_game(game);
	} while (game_over(game) == NEW_GAME);
	free_game(game);
}

