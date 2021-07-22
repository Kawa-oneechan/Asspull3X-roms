#include "../ass.h"
IBios* interface;

extern void KeyboardTest();

void WaitForKey()
{
	TEXT->SetCursorPosition(0, 28);
	printf("Press any key to continue.");
	while (REG_KEYIN != 0) { vbl(); }
	while (REG_KEYIN == 0) { vbl(); }
	while (REG_KEYIN != 0) { vbl(); }
}

int main(void)
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_320 | SMODE_BOLD);
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();

	KeyboardTest();
	WaitForKey();

	TEXT->SetTextColor(0, 15);
	printf("Welcome to the Asspull \x96\xD7 testing suite.\n");
	TEXT->SetTextColor(0, 7);
	printf("Font test");

	for (int row = 0; row < 16; row++)
	{
		for (int col = 0; col < 16; col++)
		{
			((int16_t*)MEM_VRAM)[((row + 4)  * 40) + col + 2] = ((row * 16 + col) << 8) | 0x0F;
		}
	}
	WaitForKey();

	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->ClearScreen();

	printf("Dithering test 1");

	for (int row = 0; row < 16; row++)
	{
		TEXT->SetCursorPosition(2, 2 + row);
		for (int col = 0; col < 16; col++)
		{
			TEXT->SetTextColor(row, col);
			printf(" %X%X ", row, col);
		}
	}
	TEXT->SetTextColor(0, 7);
	WaitForKey();
	TEXT->ClearScreen();

	printf("Dithering test 2");
	for (int row = 0; row < 16; row++)
	{
		TEXT->SetCursorPosition(2, 2 + row);
		for (int col = 0; col < 16; col++)
		{
			TEXT->SetTextColor(row, col);
			TEXT->WriteChar(0x20);
			TEXT->WriteChar(0x80);
			TEXT->WriteChar(0x81);
			TEXT->WriteChar(0x82);
		}
	}
	TEXT->SetTextColor(0, 7);
	WaitForKey();
}
