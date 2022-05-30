#include "../ass.h"
#include "../ass-midi.h"
IBios* interface;

extern const TImageFile piano;
extern const uint8_t tinyTiles[576];

static const char sctopit[] = {
//   0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
	 0,  0,  0, 25, 27,  0, 30, 32, 34,  0, 37, 39,  0, 42,  0, 00,  // 0x00
	24, 26, 28, 29, 31, 33, 35, 36, 38, 40, 41, 43,  0,  0,  0, 13,  // 0x10
	15,  0, 18, 20, 22,  0, 25, 27,  0,  0,  0,  0, 12, 14, 16, 17,  // 0x20
	19, 21, 23, 24, 26, 28,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x30
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x40
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x50
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x60
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x70
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x80
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x90
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xa0
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xb0
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xc0
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xd0
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xe0
	 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xf0
};

static const char* const pits[] = {
	"C-1","C#-1","D-1","D#-1","E-1","F-1","F#-1","G-1","G#-1","A-1","A#-1","B-1",
	"C0","C#0","D0","D#0","E0","F0","F#0","G0","G#0","A0","A#0","B0",
	"C1","C#1","D1","D#1","E1","F1","F#1","G1","G#1","A1","A#1","B1",
	"C2","C#2","D2","D#2","E2","F2","F#2","G2","G#2","A2","A#2","B2",
	"C3","C#3","D3","D#3","E3","F3","F#3","G3","G#3","A3","A#3","B3",
	"C4","C#4","D4","D#4","E4","F4","F#4","G4","G#4","A4","A#4","B4",
	"C5","C#5","D5","D#5","E5","F5","F#5","G5","G#5","A5","A#5","B5",
	"C6","C#6","D6","D#6","E6","F6","F#6","G6","G#6","A6","A#6","B6",
	"C7","C#7","D7","D#7","E7","F7","F#7","G7","G#7","A7","A#7","B7",
	"C8","C#8","D8","D#8","E8","F8","F#8","G8","G#8","A8","A#8","B8",
	"C9","C#9","D9","D#9","E9","F9","F#9","G9","G#9","A9","A#9","B9",
};

#define IVORY 0x7FFF
#define EBONY 0x0C63
#define KEYON 0x005D

#define KEYPALSTART 16
#define NUMKEYS 32
//1 means that key is black.
static const char colors[] = { 0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0,0,1,0,1,0,0,1,0,1,0,1,0 };

static const char* const programNames[] =
{
	"Acou.Piano",
	"BritePiano",
	"Elec.Piano",
	"Honky Tonk",
	"ElecPiano1",
	"ElecPiano2",
	"Harpsicord",
	"Clavinet",
	"Celesta",
	"Glockenspl",
	"Music Box",
	"Vibraphone",
	"Marimba",
	"Xylophone",
	"Tube Bells",
	"Dulcimer",
	"Draw Organ",
	"Perc.Organ",
	"Rock Organ",
	"ChrchOrgan",
	"Reed Organ",
	"Accordion",
	"Harmonica",
	"Tango Acc.",
	"NylonGuitr",
	"SteelGuitr",
	"JazzGuitar",
	"CleanGuitr",
	"MutedGuitr",
	"Overdriven",
	"Distortion",
	"GHarmonics",
	"Acou. Bass",
	"FingerBass",
	"PickedBass",
	"FretlsBass",
	"SlapBass 1",
	"SlapBass 2",
	"SynthBass1",
	"SynthBass2",
	"Violin",
	"Viola",
	"Cello",
	"Contrabass",
	"TremoloStr",
	"PizzicatoS",
	"Orch. Harp",
	"Timpani",
	"StringEns1",
	"StringEns2",
	"SynthStr 1",
	"SynthStr 2",
	"Choir Aahs",
	"Voice Oohs",
	"SynthVoice",
	"Orch. Hit",
	"Trumpet",
	"Trombone",
	"Tuba",
	"MutedTrump",
	"FrenchHorn",
	"BrassSect",
	"SynthBrs 1",
	"SynthBrs 2",
	"SopranoSax",
	"Alto Sax",
	"Tenor Sax",
	"BaritonSax",
	"Oboe",
	"Engl. Horn",
	"Bassoon",
	"Clarinet",
	"Piccolo",
	"Flute",
	"Recorder",
	"Pan Flute",
	"BottleBlow",
	"Shakuhachi",
	"Whistle",
	"Ocarina",
	"SquareLead",
	"Saw Lead",
	"Calliope",
	"Chiff",
	"Charang",
	"Voice",
	"Fifths",
	"Bass+Lead",
	"NewAge Pad",
	"Warm Pad",
	"Poly Pad",
	"Choir Pad",
	"Bowed Pad",
	"Metal Pad",
	"Halo",
	"Sweep",
	"Rain",
	"Soundtrack",
	"Crystal",
	"Atmosphere",
	"Brightness",
	"Goblins",
	"Echoes",
	"SciFi",
	"Sitar",
	"Banjo",
	"Shamisen",
	"Koto",
	"Kalimba",
	"Bagpipe",
	"Fiddle",
	"Shanai",
	"TinkleBell",
	"Agogo",
	"SteelDrums",
	"Woodblock",
	"Taiko Drum",
	"Melo. Tom",
	"Synth Drum",
	"Rev Cymbal",
	"GFretNoise",
	"BrethNoise",
	"Seashore",
	"Bird Tweet",
	"Telephone",
	"Helicopter",
	"Applause",
	"Gunshot",
};

void Write(int program, int pitch)
{
	char display[32];
	char *str = display;
	int x = 19, y = 184;
	TEXT->Format(display, "%-11s %2d", programNames[program], pitch + 1);

	//TODO: Make some nice #define macros for the blitter, in ass.h.
	REG_BLITSOURCE = 11;
	REG_BLITTARGET = 0x0E000000 + (y * 320) + x;
	REG_BLITLENGTH = 348;
	REG_BLITCONTROL = BLIT_SET | BLIT_BYTE | BLIT_STRIDESKIP | BLIT_SOURCESTRIDE(58) | BLIT_TARGETSTRIDE(320);

	while(*str)
	{
		interface->DrawChar((*str++) - ' ', x, y, 12);
		x += 4;
	}
}

int main(void)
{
	REG_SCREENFADE = 31;
	DRAW->DisplayPicture((TImageFile*)&piano);
	intoff();

	//Clear out all keys
	for (int i = 0; i < NUMKEYS; i++)
	{
		PALETTE[KEYPALSTART + i] = colors[i] ? EBONY : IVORY; //live together in perfect harmony...
	}

	interface->DrawCharFont = (char*)tinyTiles;
	interface->DrawCharHeight = 6;
	int pitoff = 48;
	int program = MIDI_ACOUSTICGRANDPIANO;
	Write(program, pitoff / 12);

	DRAW->Fade(true, false);

	int lastPit = -1;
	int lastPitOffed = -1;
	int pit = 0;
	MIDI_PROGRAM(1, program);
	while(1)
	{
		uint16_t key = 0;
		for (int i = 0; i < 255; i++)
		{
			if (INP_KEYMAP[i])
			{
				key = i;
				break;
			}
		}

		if ((key & 0xFF) > 0)
		{
			if (sctopit[key] > 0 && lastPit != pit)
			{
				lastPit = pit;
				lastPitOffed = pit + pitoff;
				pit = sctopit[key];
				PALETTE[KEYPALSTART - 12 + pit] = KEYON;
				MIDI_KEYON(1, (pit + pitoff), 80);
			}
			else if (key == 0xCB)
			{
				if (pitoff > 0) pitoff -= 12;
				Write(program, pitoff / 12);
				while (INP_KEYMAP[key]) { vbl(); }
			}
			else if (key == 0xCD)
			{
				if (pitoff < 108) pitoff += 12;
				Write(program, pitoff / 12);
				while (INP_KEYMAP[key]) { vbl(); }
			}
			else if (key == 0xD0)
			{
				if (program == 0) program = 128;
				program--;
				MIDI_PROGRAM(1, program);
				Write(program, pitoff / 12);
				while (INP_KEYMAP[key]) { vbl(); }
			}
			else if (key == 0xC8)
			{
				program++;
				if (program == 128) program = 0;
				MIDI_PROGRAM(1, program);
				Write(program, pitoff / 12);
				while (INP_KEYMAP[key]) { vbl(); }
			}
		}
		else if (lastPit > -1)
		{
			PALETTE[KEYPALSTART - 12 + lastPit] = colors[lastPit] ? EBONY : IVORY;
			MIDI_KEYOFF(1, lastPitOffed, 80);
			lastPit = -1;
		}
		vbl();
	}
}
