#include "../ass.h" //for the names

const char track1[] =
{
//------------------- 1 ||: First segment, loop twice
	MIDI_B4,  8,
	MIDI_E5,  16,
	MIDI_Ds5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_B4,  16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Ds5, 16,
	MIDI_E5,  8,
	MIDI_B4,  8,
//------------------- 2
	MIDI_B4,  8,
	MIDI_E5,  16,
	MIDI_Ds5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_B4,  16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Ds5, 16,
	MIDI_E5,  8,
	MIDI_B4,  8,
//------------------- 3
	MIDI_Cs5, 8,
	MIDI_E5,  16,
	MIDI_Ds5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Cs5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Ds5, 16,
	MIDI_E5,  8,
	MIDI_Cs5, 8,
//------------------- 4
	MIDI_Cs5, 8,
	MIDI_E5,  16,
	MIDI_Ds5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Cs5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Fs5, 16,
	MIDI_Gs5, 8,
	MIDI_E5,  8,
//------------------- 5 :|
	MIDI_B4,  8,
	MIDI_E5,  16,
	MIDI_Ds5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_B4,  16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Ds5, 16,
	MIDI_E5,  8,
	MIDI_B4,  8,
//------------------- 6
	MIDI_B4,  8,
	MIDI_E5,  16,
	MIDI_Ds5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_B4,  16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Ds5, 16,
	MIDI_E5,  8,
	MIDI_B4,  8,
//------------------- 7
	MIDI_Cs5, 8,
	MIDI_E5,  16,
	MIDI_Ds5, 16,
	0,        16,
	MIDI_Cs5, 8,
	MIDI_B4,  16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Ds5, 16,
	MIDI_E5,  8,
	MIDI_Cs5, 8,
//------------------- 8
	MIDI_Cs5, 8,
	MIDI_E5,  16,
	MIDI_Ds5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Cs5, 16,
	0,        16,
	MIDI_E5,  8,
	MIDI_Fs5, 16,
	MIDI_Gs5, 8,
	MIDI_E5,  8,
//------------------- 9 :|: Second segment, also twice.
	0,        8,
	MIDI_Gs4, 16,
	MIDI_B4,  16,
	MIDI_B4,  16 | 0x80, //tied
	MIDI_Ds5, 8,
	MIDI_E5,  16,
	MIDI_E5,  8 | 0x80, //tied
	MIDI_Ds5, 8,
	0,        4,
//------------------- 10
	0,        8,
	MIDI_Gs4, 16,
	MIDI_B4,  16,
	MIDI_B4,  16 | 0x80, //tied
	MIDI_Ds5, 8,
	MIDI_E5,  16,
	MIDI_E5,  8 | 0x80, //tied
	MIDI_Ds5, 8,
	0,        4,
//------------------- 11
	0,        8,
	MIDI_Cs5, 16,
	MIDI_E5,  16,
	MIDI_E5,  16 | 0x80, //tied
	MIDI_Fs5, 8,
	MIDI_Gs5, 16,
	MIDI_Gs5, 8 | 0x80, //tied
	MIDI_Fs5, 8,
	0,        4,
//------------------- 12
	0,        8,
	MIDI_Cs5, 16,
	MIDI_E5,  16,
	MIDI_E5,  16 | 0x80, //tied
	MIDI_Fs5, 8,
	MIDI_Gs5, 16,
	MIDI_Gs5, 8 | 0x80, //tied
	MIDI_Fs5, 8,
	0,        4,
//------------------- 13 :|
	0,        8,
	MIDI_Gs4, 16,
	MIDI_B4,  16,
	MIDI_B4,  16 | 0x80, //tied
	MIDI_Ds5, 8,
	MIDI_E5,  16,
	MIDI_E5,  8 | 0x80, //tied
	MIDI_Ds5, 8,
	0,        4,
//------------------- 14
	0,        8,
	MIDI_Gs4, 16,
	MIDI_B4,  16,
	MIDI_B4,  16 | 0x80, //tied
	MIDI_Ds5, 8,
	MIDI_E5,  16,
	MIDI_E5,  8 | 0x80, //tied
	MIDI_Ds5, 8,
	0,        4,
//------------------- 15
	0,        8,
	MIDI_Cs5, 16,
	MIDI_E5,  16,
	MIDI_E5,  16 | 0x80, //tied
	MIDI_Fs5, 8,
	MIDI_Gs5, 16,
	MIDI_Gs5, 8 | 0x80, //tied
	MIDI_Fs5, 8,
	0,        4,
//------------------- 16
	0,        8,
	MIDI_Cs5, 16,
	MIDI_E5,  16,
	MIDI_E5,  16 | 0x80, //tied
	MIDI_Fs5, 8,
	MIDI_Gs5, 16,
	MIDI_Gs5, 8 | 0x80, //tied
	MIDI_Fs5, 8,
	0,        4,
//------------------- :|
	1 //repeat
};

const char track2[] =
{
//------------------- 1 ||: First segment, loop twice
	MIDI_E3,  8,
	MIDI_E3,  16 | 0x80, //dotted
	MIDI_B2,  16,
	MIDI_B2,  8,
	MIDI_B2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	0,        16,
	MIDI_E3,  8,
	MIDI_B2,  16,
	MIDI_B2,  4,
//------------------- 2
	MIDI_E3,  8,
	MIDI_E3,  16 | 0x80, //dotted
	MIDI_B2,  16,
	MIDI_B2,  8,
	MIDI_B2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	0,        16,
	MIDI_E3,  8,
	MIDI_B2,  16,
	MIDI_B2,  4,
//------------------- 3
	MIDI_A2,  8,
	MIDI_A2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	MIDI_E3,  8,
	MIDI_E3,  16 | 0x80, //dotted
	MIDI_A2,  16,
	0,        16,
	MIDI_A2,  8,
	MIDI_E3,  16,
	MIDI_E3,  4,
//------------------- 4
	MIDI_A2,  8,
	MIDI_A2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	MIDI_E3,  8,
	MIDI_E3,  16 | 0x80, //dotted
	MIDI_A2,  16,
	0,        16,
	MIDI_A2,  8,
	MIDI_E3,  16,
	MIDI_E3,  4,
//------------------- 5
	MIDI_E3,  8,
	MIDI_E3,  16 | 0x80, //dotted
	MIDI_B2,  16,
	MIDI_B2,  8,
	MIDI_B2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	0,        16,
	MIDI_E3,  8,
	MIDI_B2,  16,
	MIDI_B2,  4,
//------------------- 6
	MIDI_E3,  8,
	MIDI_E3,  16 | 0x80, //dotted
	MIDI_B2,  16,
	MIDI_B2,  8,
	MIDI_B2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	0,        16,
	MIDI_E3,  8,
	MIDI_B2,  16,
	MIDI_B2,  4,
//------------------- 7
	MIDI_A2,  8,
	MIDI_A2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	MIDI_E3,  8,
	MIDI_E3,  16 | 0x80, //dotted
	MIDI_A2,  16,
	0,        16,
	MIDI_A2,  8,
	MIDI_E3,  16,
	MIDI_E3,  4,
//------------------- 8
	MIDI_A2,  8,
	MIDI_A2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	MIDI_E3,  8,
	MIDI_E3,  16 | 0x80, //dotted
	MIDI_A2,  16,
	0,        16,
	MIDI_A2,  8,
	MIDI_E3,  16,
	MIDI_E3,  4,
//------------------- 9 :|: Second segment, also twice.
	MIDI_E2,  8,
	MIDI_E2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	MIDI_E3,  8,
	MIDI_B2,  16,
	MIDI_B2,  16,
	0,        16,
	MIDI_E2,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  16,
//------------------- 10
	MIDI_E2,  8,
	MIDI_E2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	MIDI_E3,  8,
	MIDI_B2,  16,
	MIDI_B2,  16,
	0,        16,
	MIDI_E2,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  16,
//------------------- 11
	MIDI_A2,  8,
	MIDI_A2,  16 | 0x80, //dotted
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Gs2, 16,
	MIDI_A2,  16,
	0,        16,
	MIDI_A2,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  16,
//------------------- 12
	MIDI_A2,  8,
	MIDI_A2,  16 | 0x80, //dotted
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Gs2, 16,
	MIDI_A2,  16,
	0,        16,
	MIDI_A2,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  16,
//------------------- 13 :|
	MIDI_E2,  8,
	MIDI_E2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	MIDI_E3,  8,
	MIDI_B2,  16,
	MIDI_B2,  16,
	0,        16,
	MIDI_E2,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  16,
//------------------- 14
	MIDI_E2,  8,
	MIDI_E2,  16 | 0x80, //dotted
	MIDI_E3,  16,
	MIDI_E3,  8,
	MIDI_B2,  16,
	MIDI_B2,  16,
	0,        16,
	MIDI_E2,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  16,
//------------------- 15
	MIDI_A2,  8,
	MIDI_A2,  16 | 0x80, //dotted
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Gs2, 16,
	MIDI_A2,  16,
	0,        16,
	MIDI_A2,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  16,
//------------------- 16
	MIDI_A2,  8,
	MIDI_A2,  16 | 0x80, //dotted
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Gs2, 16,
	MIDI_A2,  16,
	0,        16,
	MIDI_A2,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  8,
	MIDI_Ds3, 16,
	MIDI_E3,  16,
//------------------- :|
	1 //repeat
};

//Leaving out the drumtrack -- source material is weird.
/*
const char track3[] =
{
//------------------- 1 ||: First segment, loop twice
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 2
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 3
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 4
	0,   4,
	???, 8,
	???, 16,
	???, 16,
	0,   16,
	???, 8,
	???, 16,
	???, 8,
	???, 16,
	???, 16,
//------------------- 5 :|
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 6
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 7
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 8
	0,   4,
	???, 8,
	???, 16,
	???, 16,
	0,   16,
	???, 8,
	???, 16,
	???, 8,
	???, 16,
	???, 16,
//------------------- 9 :|: Second segment, also twice.
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 10
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 11
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 12
	0,   4,
	???, 8,
	???, 16,
	???, 16,
	0,   16,
	???, 8,
	???, 16,
	???, 8,
	???, 16,
	???, 16,
//------------------- 13 :|
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 14
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 15
	0,   4,
	???, 4,
	0,   4,
	???, 4,
//------------------- 16
	0,   4,
	???, 8,
	???, 16,
	???, 16,
	0,   16,
	???, 8,
	???, 16,
	???, 8,
	???, 16,
	???, 16,
//------------------- :|
	1 //repeat
};
*/

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
	4, MIDI_ELECTRICPIANO1,
	5, MIDI_LEAD1SQUARE,
	//10, 0 //drums
};
