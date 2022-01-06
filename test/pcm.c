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
	REG_PCMOFFSET = (unsigned int)soundLoop + 4;
	REG_PCMLENGTH = *(unsigned int*)soundLoop | PCM_REPEAT;
	WaitForKey();
	REG_PCMLENGTH = 0;
	TEXT->SetCursorPosition(0, 3);
	TEXT->Write("The loop should have stopped.");
	WaitForKey();
	TEXT->SetCursorPosition(0, 4);
	TEXT->Write("Playing a sample once.");
	REG_PCMOFFSET = (unsigned int)soundOne + 4;
	REG_PCMLENGTH = *(unsigned int*)soundOne;
	WaitForKey();
}
