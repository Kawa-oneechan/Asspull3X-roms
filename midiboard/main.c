#include "../ass.h"
IBios* interface;

//TODO: add graphical keyboard with highlighting keys

static const char sctopit[] = {
//   0   1   2   3   4   5   6   7   8   9   a   b   c   d   e   f
	 0,  0,  0, 24, 26,  0, 30, 32, 34,  0, 37, 39,  0, 90,  0, 00,  // 0x00
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

int main(void)
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();

	short lastKey = 0;
	int pitoff = 48;
	MIDI_PROGRAM(1, MIDI_ACOUSTICGRANDPIANO);
	while(1)
	{
		unsigned short key = REG_KEYIN;
		TEXT->SetCursorPosition(12, 6);

		if ((key & 0xFF) > 0)
		{
			if (lastKey != key)
			{
				lastKey = key;
				if (sctopit[key] > 0)
				{
					int pit = sctopit[key] + pitoff;
					MIDI_KEYON(1, pit, 80);
					TEXT->SetTextColor(1, 15);
					printf("%4s", pits[pit]);
				}
				else if(key == 0xD0)
				{
					if (pitoff > 0) pitoff -= 12;
				}
				else if(key == 0xC8)
				{
					if (pitoff < 108) pitoff += 12;
				}
			}
		}
		else
			lastKey = 0;
		vbl();
	}
}
