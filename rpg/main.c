#include "rpg.h"
IBios* interface;

int lastInput;

void getInput()
{
	lastInput = INP_KEYIN;
	if ((INP_JOYPAD1 & BUTTON_UP) || INP_KEYMAP[KEYSCAN_UP]) lastInput = KEYSCAN_UP;
	else if ((INP_JOYPAD1 & BUTTON_RIGHT) || INP_KEYMAP[KEYSCAN_RIGHT]) lastInput = KEYSCAN_RIGHT;
	else if ((INP_JOYPAD1 & BUTTON_DOWN) || INP_KEYMAP[KEYSCAN_DOWN]) lastInput = KEYSCAN_DOWN;
	else if ((INP_JOYPAD1 & BUTTON_LEFT) || INP_KEYMAP[KEYSCAN_LEFT]) lastInput = KEYSCAN_LEFT;
	else if (INP_JOYPAD1 & BUTTON_B) lastInput = KEY_ACTION;
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

uint16_t convertRange(uint16_t v, uint16_t oldMax, uint16_t newMax)
{
	return (uint16_t)(v * newMax) / oldMax;
}

void drawBar(int x, int y, int v, int m, int l)
{
	int barDotsFilled = convertRange(v, m, l * 8);
	int barWholes = barDotsFilled / 8;
	int barFracs = barDotsFilled % 8;

	uint16_t *map = &MAP4[(y * 64) + x];
	*map++ = 0xF010;
	if (barWholes)
		for (int i = 0; i < barWholes; i++)
			*map++ = 0xF01A;
	if (barFracs)
		*map++ = 0xF012 + barFracs;
	for (int i = barWholes + (barFracs > 0); i < l; i++)
		*map++ = 0xF012;
	*map++ = 0xF011;
}

extern void startBattle();

int main(void)
{
	REG_MAPSET = 0xF0;

	MISC->SetTextMode(SMODE_TILE);
	MISC->DmaClear(TILESET, 0, 0x4000, DMA_INT);
	MISC->DmaClear(OBJECTS_A, 0, 0x1000, DMA_INT);
	MISC->DmaCopy(PALETTE + 240, (int16_t*)&uiPal, 8, DMA_INT);
	MISC->DmaCopy(PALETTE + 256 + 240, (int16_t*)&uiPal, 8, DMA_INT);

//	MISC->DmaCopy(TILESET + 0x6000, (int8_t*)&fontTiles, 0x700, DMA_INT);

	MISC->DmaCopy(TILESET + 0xC000, (int8_t*)&uiTiles, 0x100, DMA_INT);
	MISC->DmaClear(TILESET+ 0xC200, 0x88888888, 0x200 / 4, DMA_INT);
	REG_BLITSOURCE = (int32_t)uiTiles + 0x200;
	REG_BLITTARGET = (int32_t)TILESET + 0xC200;
	REG_BLITLENGTH = 0x200;
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

//	swapPartyMembers(1, 0);
//	startBattle(4);

	loadMap((Map*)testMap);
	PALETTE[0] = 0;

	drawMap();
	//saySomething("* What a year, huh?\n* Captain, it's February.", 1);
	//saySomething("0123456789012345678901234567890123\b\nSupercalifragilisticexpialidocious\nEven though the sound of it is\nsomething quite atrocious.", 0);

//	drawBar(1, 2, 40, 80, 6);

	for(;;)
	{
		updateAndDraw();
//		drawBar(1, 2, entities[0].x * entities[0].y, 256, 6);
		vbl();
		getInput();
	}
}
