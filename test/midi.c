#include "../ass.h"
extern IBios* interface;

extern void WaitForKey();

#define PRG(c, p) (0xC0 | c) | (p << 8)
#define KON(c, p, v) (0x90 | c) | (p << 8) | (v << 16)
#define KOF(c, p, v) (0x80 | c) | (p << 8) | (v << 16)
#define SUS(c, p) (0xB0 | c) | (64 << 8) | (p << 16)

const uint32_t mididata[] =
{
//	PRG(0, 24),

	KON(0, 46, 39), 0,
	KON(0, 58, 35), 4,
	KON(0, 65, 63), 2,
	SUS(0, 47), 1,
	SUS(0, 110), 1,
	SUS(0, 127), 0,

	KON(0, 70, 34), 5,
	KON(0, 74, 61), 6,
	KOF(0, 65, 63), 2,
	KOF(0, 70, 34), 5,
	KOF(0, 46, 39), 3,
	KOF(0, 58, 35), 16,
	KOF(0, 74, 61), 44,

	KON(0, 72, 66), 26,
	KOF(0, 72, 66), 2,
	KON(0, 70, 87), 29,
	KOF(0, 70, 87), 26,
	KON(0, 74, 62), 9,
	KOF(0, 74, 62), 39,
	KON(0, 55, 64), 3,
	SUS(0, 0), 2,

	KON(0, 67, 76), 4,
	KON(0, 71, 47), 3,
	KON(0, 74, 70), 1,
	SUS(0, 38), 0,
	SUS(0, 107), 1,
	SUS(0, 127), 3,

	KOF(0, 67, 76), 4,
	KOF(0, 71, 47), 7,
	KOF(0, 74, 70), 89,
	KOF(0, 55, 64), 5,

	KON(0, 79, 38), 25,
	KOF(0, 79, 38), 87,
	KON(0, 48, 36), 3,
	KON(0, 60, 47), 5,
	KON(0, 67, 40), 3,
	KON(0, 72, 40), 2,
	SUS(0, 0), 5,
	KON(0, 79, 35), 5,
	SUS(0, 25), 1,
	SUS(0, 103), 1,
	SUS(0, 127), 1,
	KOF(0, 67, 40), 4,
	KOF(0, 72, 40), 11,
	KOF(0, 48, 36), 2,
	KOF(0, 60, 47), 4,
	KOF(0, 79, 38), 154,
	SUS(0, 0), 100,

	0xFFFFFFFF
};

static const uint32_t *_midiptr;
static uint16_t _midiwait;

static void MIDI_Service()
{
	uint32_t value;

	if (!_midiptr)
	{
		printf("done");
		return;
	}

	while (!_midiwait)
	{
		value = *_midiptr++;
		_midiwait = *_midiptr++;
		//_midiwait /= 2;
		//_midiwait = (_midiwait + 127) / 128;
		if (value == 0xFFFFFFFF)
		{
			_midiptr = NULL;
			_midiwait = 0;
			return;
		}
		REG_MIDIOUT = (value >> 0)  & 0xFF;
		REG_MIDIOUT = (value >> 8)  & 0xFF;
		REG_MIDIOUT = (value >> 16) & 0xFF;
	}
	_midiwait--;
}

void MIDI_Play()
{
	if (!_midiptr)
		return;
	//for (int i = 0; i < 16; i++)
	MIDI_Service();
}


void MIDITest()
{
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	TEXT->Write("MIDI music test");

	TEXT->SetCursorPosition(0, 2);
	inton();
	REG_MIDIOUT = 0xC0; REG_MIDIOUT = 24;
	interface->VBlank = MIDI_Play;
	_midiptr = mididata;
	_midiwait = 0;

	WaitForKey();
	interface->VBlank = 0;
}
