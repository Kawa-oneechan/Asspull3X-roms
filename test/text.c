#include "../ass.h"
IBios* interface;

extern void WaitForKey();

const char customBits[96] =
{
	0x00, 0x18, 0x18, 0x18, 0x8C, 0x8C, 0x8C, 0xFC, 0xFE, 0xC6, 0xC6, 0x63, 0x63, 0x63, 0x63, 0x00,
	0x00, 0x03, 0x03, 0x03, 0xE1, 0xF1, 0x39, 0x18, 0x9C, 0xFC, 0xFC, 0x06, 0x06, 0xFE, 0x7C, 0x00,
	0x00, 0x00, 0xC0, 0xC0, 0x61, 0x63, 0x33, 0x33, 0x1B, 0x1B, 0x18, 0x0C, 0x0D, 0xFC, 0xF8, 0x00,
	0x00, 0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0C, 0x0C, 0x0C, 0x06, 0x06, 0xFF, 0xFC, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF8, 0x98, 0x8C, 0x8C, 0x86, 0x86, 0xC6, 0xFD, 0x78, 0x00,
	0x00, 0x00, 0x30, 0x70, 0x78, 0x78, 0x39, 0x39, 0x1D, 0x1D, 0x0D, 0x0D, 0x00, 0x06, 0x06, 0x00,
};

void TextTest()
{
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	TEXT->Write("Font test - Bold");
	for (int row = 0; row < 8; row++)
	{
		for (int col = 0; col < 32; col++)
		{
			((int16_t*)MEM_VRAM)[((row + 4)  * 80) + col + 2] = ((row * 32 + col) << 8) | 0x0F;
		}
	}
	WaitForKey();
	MISC->SetTextMode(SMODE_240);
	TEXT->SetCursorPosition(0, 0);
	TEXT->Write("Font test - Thin");
	WaitForKey();
	TEXT->SetCursorPosition(0, 0);
	TEXT->Write("Font test - Customized characters");
	MISC->DmaCopy(TEXTFONT + 16 + 0x1000, customBits, 96, DMA_BYTE);
	WaitForKey();

	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->ClearScreen();
	TEXT->Write("Color attribute test - Bright");

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
	TEXT->SetCursorPosition(0, 0);
	TEXT->Write("Color attribute test - Blinking");
	WaitForKey();
	REG_SCREENMODE &= ~SMODE_BLINK;
	TEXT->ClearScreen();

	TEXT->Write("Dithering test");
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
	WaitForKey();
}
