#include "../ass.h"

IBios* interface;

int main(void)
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetCursorPosition(0, 0);
	TEXT->SetTextColor(0, 7);
	REG_SCREENFADE = 0;

	printf("This ran from disk!\n");

	while(1);
}
