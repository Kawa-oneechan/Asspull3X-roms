#include "../ass.h"
extern IBios* interface;

extern void WaitForKey();

extern const unsigned char soundLoop[], soundOne[];

void PCMTest()
{
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	TEXT->Write("PCM audio test");

	TEXT->SetCursorPosition(0, 2);
	TEXT->Write("Playing a sample on loop.");
	PCMOFFSET[0] = (unsigned int)soundLoop + 4;
	PCMLENGTH[0] = *(unsigned int*)soundLoop | PCM_REPEAT;
	WaitForKey();
	PCMLENGTH[0] = 0;
	TEXT->SetCursorPosition(0, 3);
	TEXT->Write("The loop should have stopped.");
	WaitForKey();
	TEXT->SetCursorPosition(0, 4);
	TEXT->Write("Playing a sample once.");
	PCMOFFSET[0] = (unsigned int)soundOne + 4;
	PCMLENGTH[0] = *(unsigned int*)soundOne;
	WaitForKey();
	TEXT->SetCursorPosition(0, 5);
	TEXT->Write("Playing the loop again...");
	PCMOFFSET[0] = (unsigned int)soundLoop + 4;
	PCMLENGTH[0] = *(unsigned int*)soundLoop | PCM_REPEAT;
	WaitForKey();
	TEXT->Write(" and a second sample on top!");
	PCMOFFSET[1] = (unsigned int)soundOne + 4;
	PCMLENGTH[1] = *(unsigned int*)soundOne;
	WaitForKey();
	PCMLENGTH[0] = 0;
	PCMLENGTH[1] = 0;

	TEXT->SetCursorPosition(0, 6);
	TEXT->Write("Playing the loop yet again but panned left...");
	PCMOFFSET[0] = (unsigned int)soundLoop + 4;
	PCMLENGTH[0] = *(unsigned int*)soundLoop | PCM_REPEAT;
	PCMVOLUME[0] = 255;
	PCMVOLUME[1] = 0;
	WaitForKey();
	TEXT->Write(" and panning it to and fro.");

	for (int r = 0; r < 2; r++);
	{
		for (int i = 0; i < 256; i++)
		{
			PCMVOLUME[0] = 255 - i;
			PCMVOLUME[1] = i;
			MISC->WaitForVBlanks(4);
		}
		MISC->WaitForVBlanks(128);
		for (int i = 0; i < 256; i++)
		{
			PCMVOLUME[0] = i;
			PCMVOLUME[1] = 255 - i;
			MISC->WaitForVBlanks(4);
		}
		MISC->WaitForVBlanks(128);
	}

	PCMLENGTH[0] = 0;
	WaitForKey();
}
