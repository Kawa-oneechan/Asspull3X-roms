#include "../ass.h"
extern IBios* interface;

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
}
