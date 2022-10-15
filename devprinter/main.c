#include "../ass.h"
IBios* interface;

void LP_Print(const char* text)
{
	while (*text)
		*interface->LinePrinter = *text++;
}
//------

//The rest is *mostly* just HelloWorld.
int main(void)
{
	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();
	if (interface->LinePrinter == 0)
	{
		printf("No line printer attached.");
		return;
	}
	printf("Look at the printer.");

	LP_Print("Hello to the lineprinter!\n");
	LP_Print("This has some " "\x1B" "Ebold text." "\x1B" "e.\n");
	LP_Print("This has some " "\x1B" "Uunderlined text." "\x1B" "u.\n");
	LP_Print("This has " "\x1B" "E" "\x1B" "Uboth at once." "\x1B" "u" "\x1B" "e.\n");
	LP_Print("        \\   ^__^\n");
	LP_Print("         \\  (oo)\\_______\n");
	LP_Print("            (__)\\       )\\/\\\n");
	LP_Print("                ||----w |\n");
	LP_Print("                ||     ||\n");
}
