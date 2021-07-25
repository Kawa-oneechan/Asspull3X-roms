#include "../ass.h" //for the names

const char track1[] =
{
	//PIANO 1
//------------------- 1 ||: Repeat until part 2, but let's just only have part 1
	MIDI_C3,  8,
	MIDI_C4,  8,
	MIDI_C3,  8,
	MIDI_C4,  8,
	MIDI_Ds3, 8,
	MIDI_Ds4, 8,
	MIDI_Ds3, 8,
	MIDI_Ds4, 8,
//------------------- 2
	MIDI_Gs2, 8,
	MIDI_Gs3, 8,
	MIDI_Gs2, 8,
	MIDI_Gs3, 8,
	MIDI_As2, 8,
	MIDI_As3, 8,
	MIDI_As2, 8,
	MIDI_As3, 8,
//------------------- :|
	1 //repeat
};

const char track2[] =
{
	//ORGAN
//------------------- 1
	0,        1,
//------------------- 2
	0,        1,
//------------------- 3
	0,        1,
//------------------- 4
	0,        2,
	0,        3,
	MIDI_D5,  8,
	MIDI_G5,  8,
	MIDI_As5, 8,
//------------------- 5 |:
	MIDI_A5,  8,
	MIDI_D5,  8,
	MIDI_G5,  8,
	MIDI_D5,  8,
	MIDI_F5,  8,
	MIDI_D5,  8,
	MIDI_G5,  8,
	MIDI_D5,  8,
//------------------- 6
	MIDI_A5,  8,
	MIDI_D5,  8,
	MIDI_G5,  8,
	MIDI_D5,  8,
	0,        8,
	MIDI_D5,  8,
	MIDI_G5,  8,
	MIDI_As5, 8,
//------------------- 7
	MIDI_A5,  8,
	MIDI_D5,  8,
	MIDI_G5,  8,
	MIDI_D5,  8,
	MIDI_F5,  8,
	MIDI_D5,  8,
	MIDI_G5,  8,
	MIDI_D5,  8,
//------------------- 8
	MIDI_A5,  8,
	MIDI_D5,  8,
	MIDI_As5, 8,
	MIDI_C6,  8,
	0,        8,
	MIDI_D5,  8,
	MIDI_G5,  8,
	MIDI_As5, 8,
//------------------- :|
	2, 8 //goto
};

const char* const musicTracks[] =
{
	track1,
	track2,
	//track3
};

const char musicSettings[] =
{
	2, //two tracks

	//reserve first few channels for sound effects
	4, MIDI_ACOUSTICGRANDPIANO,
	5, MIDI_CHURCHORGAN,
	//10, 0 //drums
};
