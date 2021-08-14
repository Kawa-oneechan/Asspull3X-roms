#include "../ass.h"
IBios* interface;

extern void WaitForKey();

void TextTest()
{
	TEXT->SetTextColor(0, 7);
	TEXT->Write("Font test");
	for (int row = 0; row < 16; row++)
	{
		for (int col = 0; col < 16; col++)
		{
			((int16_t*)MEM_VRAM)[((row + 4)  * 80) + col + 2] = ((row * 16 + col) << 8) | 0x0F;
		}
	}
	WaitForKey();

	TEXT->ClearScreen();

	TEXT->Write("Dithering test 1");

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
	REG_SCREENMODE |= SMODE_BLINK;
	WaitForKey();
	REG_SCREENMODE &= ~SMODE_BLINK;
	TEXT->ClearScreen();

	TEXT->Write("Dithering test 2");
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
