#include "../ass.h"
extern IBios* interface;

extern void IMF_Play();
extern void IMF_AudioT(void*);
extern int IMF_LoadSong(const uint16_t *sauce, bool loop);
extern const uint16_t imfData[];

const uint8_t keen60[] =
{
	//Length
	0x1E, 0x00, 0x00, 0x00,
	//Priority
	0x50, 0x00,
	//OPL settings
	0x04, 0x81, 0xC0, 0x0E, 0xB8, 0xE5, 0xFF, 0xF8,
	0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	//Octave
	0x06,
	//Pitch data
	0xC8, 0xC8, 0xC8, 0xC8, 0x00, 0x00, 0xDA, 0xDA,
	0xDA, 0xDA, 0xDA, 0x00, 0x00, 0x00, 0xE6, 0xE6,
	0xE6, 0xE6, 0xE6, 0xE6, 0x00, 0x00, 0x00, 0x00,
	0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0xF2, 0x00, 0x00
};

const uint8_t keen61[] =
{
	0x50, 0x00, 0x00, 0x00,
	0x55, 0x00,
	0x21, 0x21, 0x16, 0x00, 0x71, 0x81, 0xAE, 0x9E,
	0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x06,
	0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0x00,
	0x00, 0x00, 0x00, 0xD6, 0xD6, 0xD6, 0xD6, 0xD6,
	0xD6, 0xD6, 0xD6, 0xD6, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8, 0xB8,
	0xB8, 0xB8, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0x00
};

const uint8_t keen64[] =
{
	0xB2, 0x00, 0x00, 0x00,
	0x63, 0x00,
	0x21, 0x11, 0x11, 0x00, 0xA3, 0xC4, 0x43, 0x22,
	0x02, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x05,
	0xBF, 0xBF, 0xBF, 0xBF, 0x00, 0x00, 0x00, 0x00,
	0xBF, 0xBF, 0xBF, 0xBF, 0xBF, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0xBF, 0xBF, 0xBF, 0xBF, 0xBF,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xBF, 0xBF,
	0xBF, 0xBF, 0xBF, 0xBF, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x99, 0x99, 0x99, 0x99, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x99, 0x99, 0x99, 0x99, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
	0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
	0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0x99,
	0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99, 0x99,
	0x99, 0x99, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3,
	0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0xC3, 0x00, 0x00,
	0x00, 0x00, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
	0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
	0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
	0xFD, 0xFD, 0x00
};

const uint8_t keen66[] =
{
	0x23, 0x00, 0x00, 0x00,
	0x5A, 0x00,
	0x21, 0x21, 0x16, 0x00, 0x71, 0x81, 0xAE, 0x9E,
	0x00, 0x00, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x04,
	0xC1, 0xC3, 0xC5, 0xC6, 0xC8, 0xCA, 0xCC, 0xCD,
	0xD1, 0xD4, 0xD6, 0xD8, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xDB, 0xDA, 0xD8, 0xD6, 0xCF, 0xCD,
	0xC8, 0xC5, 0xC1, 0xBA, 0xB7, 0xB3, 0xAE, 0xA9,
	0xA0, 0x99, 0x99, 0x00
};

static void WaitForKey()
{
	//TEXT->SetCursorPosition(0, 28);
	//printf("Press any key to continue.");
	const char prompt[] = "Press 1-4 for sounds, any other key to continue.";
	const char spinner[] = "\xB8\x08+*+\x08\xB8    ";
	int spin = 0;
	for (int i = 0; i < 48; i++)
		TEXTMAP[(29 * 80) + i] = 0x07 | (prompt[i] << 8);
	while (true)
	{
		int key;
		while ((key = INP_KEYIN) == 0)
		{
			TEXTMAP[(29 * 80) + 51] = 0x0E | (spinner[((spin++) >> 4) % 11] << 8);
			TEXTMAP[(29 * 80) + 50] = 0x0E | (spinner[((spin >> 4) + 1) % 11] << 8);
			TEXTMAP[(29 * 80) + 49] = 0x0E | (spinner[((spin >> 4) + 2) % 11] << 8);
			vbl();
		}
		if (key == 2) IMF_AudioT((void*)&keen60);
		else if (key == 3) IMF_AudioT((void*)&keen61);
		else if (key == 4) IMF_AudioT((void*)&keen64);
		else if (key == 5) IMF_AudioT((void*)&keen66);
		else break;
	}
	for (int i = 0; i < 30; i++)
		TEXTMAP[(29 * 80) + i] = 0x2007;
}

void OPLTest()
{
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	TEXT->Write("OPL3 music test\n\nPlaying: lot_bon by Nurykabe, CC-BY.");

	interface->vBlank = IMF_Play;
	IMF_LoadSong(imfData, true);
	WaitForKey();
	interface->vBlank = 0;

	MISC->OplReset();
}
