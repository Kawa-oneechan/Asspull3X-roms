#include "rpg.h"
IBios* interface;

int lastInput;

void getInput()
{
	lastInput = INP_KEYIN;
	if ((INP_JOYPAD1 & 1) || INP_KEYMAP[KEY_UP]) lastInput = KEY_UP;
	else if ((INP_JOYPAD1 & 2) || INP_KEYMAP[KEY_RIGHT]) lastInput = KEY_RIGHT;
	else if ((INP_JOYPAD1 & 4) || INP_KEYMAP[KEY_DOWN]) lastInput = KEY_DOWN;
	else if ((INP_JOYPAD1 & 8) || INP_KEYMAP[KEY_LEFT]) lastInput = KEY_LEFT;
	else if (INP_JOYPAD1 & 0x20) lastInput = KEY_ACTION;
}

void waitForActionKey()
{
	while (INP_KEYMAP[KEY_ACTION]) { vbl(); }
	while (!INP_KEYMAP[KEY_ACTION]) { vbl(); }
	lastInput = 0;
}

void drawWindow(int l, int t, int w, int h)
{
	for (int i = t + 1; i < t + h - 1; i++)
	{
		MAP4[(i * 64) + l] = 0xF00A;
		MAP4[(i * 64) + l + w - 1] = 0xF00C;
		for (int j = l + 1; j < l + w - 1; j++)
			MAP4[(i * 64) + j] = 0xF001;
	}
	for (int i = l + 1; i < l + w - 1; i++)
	{
		MAP4[(t * 64) + i] = 0xF00B;
		MAP4[((t + h - 1) * 64) + i] = 0xF00D;
	}
	MAP4[(t * 64) + l] = 0xF006;
	MAP4[(t * 64) + l + w - 1] = 0xF007;
	MAP4[((t + h - 1) * 64) + l] = 0xF008;
	MAP4[((t + h - 1) * 64) + l + w - 1] = 0xF009;
}

void eraseWindow(int l, int t, int w, int h)
{
	for (int i = t; i < t + h; i++)
	{
		for (int j = l; j < l + w; j++)
			MAP4[(i * 64) + j] = 0;
	}
}

void drawString(int x, int y, const char* string)
{
	char *c = (char*)string;
	int pos = (y * 64) + x;
	int t = 0;
	while (*c)
	{
		if (*c == '\n')
		{
			y += 2;
			pos = (y * 64) + x;
			c++;
			continue;
		}
		t = (*c - ' ' + 16) * 2;
		t |= 0xF000;
		MAP4[pos +  0] = t;
		MAP4[pos + 64] = t + 1;
		pos++;
		c++;
	}
}

int main(void)
{
	REG_MAPSET = 0xF0;

	MISC->SetTextMode(SMODE_TILE);
	MISC->DmaClear(TILESET, 0, 0x4000, DMA_INT);
	MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);
	MISC->DmaCopy(PALETTE + 240, (int16_t*)&fontPal, 8, DMA_INT);
	MISC->DmaCopy(PALETTE + 256 + 240, (int16_t*)&fontPal, 8, DMA_INT);

//	MISC->DmaCopy(TILESET + 0x6000, (int8_t*)&fontTiles, 0x700, DMA_INT);

	MISC->DmaCopy(TILESET + 0xC000, (int8_t*)&fontTiles, 0x80, DMA_INT);
	MISC->DmaClear(TILESET+ 0xC400, 0x88888888, 0x3800 / 4, DMA_INT);
//	MISC->DmaCopy(TILESET + 0x6400, (int8_t*)&uiTiles + 0x400, 0x5D0, DMA_INT);
	REG_BLITSOURCE = (int32_t)fontTiles + 0x400;
	REG_BLITTARGET = (int32_t)TILESET + 0xC400;
	REG_BLITLENGTH = 0x3800;
	REG_BLITKEY = 0;
	REG_BLITCONTROL = BLIT_COPY | 0xC0 | BLIT_COLORKEY;
	REG_MAPSHIFT = 0xD5;

	MISC->DmaCopy(PALETTE + 256, (int16_t*)spritepalPal, 48, DMA_INT);

	MISC->DmaClear(MAP1, 0, WIDTH * HEIGHT, DMA_INT);
	MISC->DmaClear(MAP2, 0, WIDTH * HEIGHT, DMA_INT);
	MISC->DmaClear(MAP3, 0, WIDTH * HEIGHT, DMA_INT);
	MISC->DmaClear(MAP4, 0, WIDTH * HEIGHT, DMA_INT);

	REG_HDMASOURCE[0] = (int32_t)uiBackground;
	REG_HDMATARGET[0] = (int32_t)PALETTE + 496;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	loadMap((Map*)testMap);
	PALETTE[0] = 0;

	drawMap();
	//saySomething("* What a year, huh?\n* Captain, it's February.", 1);
	//saySomething("0123456789012345678901234567890123\b\nSupercalifragilisticexpialidocious\nEven though the sound of it is\nsomething quite atrocious.", 0);

	for(;;)
	{
		updateAndDraw();
		vbl();
		getInput();
	}
}
