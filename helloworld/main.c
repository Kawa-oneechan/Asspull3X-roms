#include "../ass.h"
IBios* interface;

int main(void)
{
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();
	printf("Hello, world!\n");
}
