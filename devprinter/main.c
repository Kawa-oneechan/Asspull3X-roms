#include "../ass.h"
IBios* interface;

//The following, with the printf statements removed, may be moved into the BIOS.
char* LP_Out = 0;
void LP_Init()
{
	printf("Looking for the line printer... ");
	LP_Out = 0;
	for (int i = 0; i < 16; i++)
	{
		uint16_t signature = *(uint16_t*)(0x02000000 + (i * 0x8000));
		if (signature == 0x4C50)
		{
			printf("Found as device #%d.\n", i);
			LP_Out = (char*)(0x02000002 + (i * 0x8000));
			return;
		}
	}
	printf("Couldn't find it.\n");
}
void LP_Print(const char* text)
{
	while (*text)
		*LP_Out = *text++;
}
//------

//The rest is *mostly* just HelloWorld.
int main(void)
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();
	printf("Hello, world!\n");

	LP_Init();
	LP_Print("Hello to the lineprinter!\n");
}
