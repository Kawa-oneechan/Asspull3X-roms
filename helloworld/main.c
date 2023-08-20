#include "../ass.h"
IBios* interface;

int main(void)
{
	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();
	printf("Hello, world!\n");
}
