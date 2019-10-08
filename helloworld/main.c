#include "../ass.h"
IBios* interface;

void WaitForKey()
{
	while (REG_KEYIN == 0);
	vbl();
	while (REG_KEYIN != 0);
}

int main(void)
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();
	TEXT->SetCursorPosition(0, 0);
	printf("Hello, world!\n");
}
