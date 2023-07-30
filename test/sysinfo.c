#include "../ass.h"

extern void WaitForKey();

void SystemInfo()
{
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	TEXT->Write("System Information\n\n");

	TEXT->Write("Devices\n\x90\x90\x90\x90\x90\x90\x90\n");
	uint8_t* devices = (uint8_t*)MEM_DEVS;
	for (char i = 0; i < 16; i++)
	{
		TEXT->Write("%2d. ", i);
		if (i == 0)
			TEXT->Write("Input controller\n");
		else if (*(int16_t*)devices == DEVICE_ID_LINEPRINTER)
		{
			interface->linePrinter = devices + 2;
			TEXT->Write("Line printer\n");
		}
		else if (*(int16_t*)devices == DEVICE_ID_DISKDRIVE)
		{
			for (int j = 0; j < 4; j++)
			{
				if (interface->io.diskToDev[j] == i)
				{
					TEXT->Write("%s drive %c:\n", *(char*)&devices[5] ? "Hard disk" : "Diskette", 'A' + j);
					break;
				}
			}
		}
		else
			TEXT->Write("----\n");
		devices += 0x8000;
	}
	TEXT->Write("\n");

	TEXT->Write("Memory\n\x90\x90\x90\x90\x90\x90\n");
	uint8_t* memTest = (uint8_t*)0x01000000;
	while (memTest < (uint8_t*)0x01400000)
	{
		TEXT->Write("0x%08X ...", memTest);
		*memTest = 42;
		vbl();
		if (*memTest != 42)
		{
			interface->io.attribs = 0x0C;
			TEXT->Write(" Something's wrong around here.");
			WaitForKey();
			return;
		}
		memTest += 1024 * 8;
		TEXT->Write("\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
	TEXT->Write("0x%08X O.K.\n", memTest);

	WaitForKey();
}
