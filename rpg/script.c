#include "rpg.h"

const uint8_t cursorTiles[] =
{
	0x00,0x00,0x30,0x33,0x00,0x00,0x23,0x22,0x00,0x30,0x26,0x62,0x30,0x30,0x22,0x22,
	0x63,0x63,0x22,0x22,0x23,0x24,0x22,0x22,0x23,0x24,0x22,0x22,0x63,0x64,0x22,0x22,
	0x03,0x00,0x00,0x00,0x32,0x00,0x00,0x00,0x44,0x33,0x33,0x00,0x22,0x22,0x22,0x03,
	0x22,0x22,0x22,0x32,0x22,0x66,0x66,0x03,0x62,0x36,0x33,0x00,0x62,0x34,0x00,0x00,
	0x63,0x64,0x26,0x22,0x63,0x44,0x66,0x66,0x43,0x43,0x64,0x66,0x30,0x30,0x44,0x44,
	0x00,0x00,0x33,0x33,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x66,0x34,0x00,0x00,0x46,0x03,0x00,0x00,0x44,0x03,0x00,0x00,0x34,0x00,0x00,0x00,
	0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x10,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x00,0x11,0x11,0x11,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x11,0x11,0x11,0x00,0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x01,0x00,0x00,0x00,0x00,
};

int dialogueBoxIsOpen, dialoguePortrait;

char playerName[16] = "Farrah";
uint32_t scriptVariables[256] =
{
	0, 0, (uint32_t)&playerName
};

void saySomething(char *what, int flags)
{
	char *c = what;

	if (!dialogueBoxIsOpen)
	{
		for (int i = 0; i < 3; i++)
		{
			drawWindow(2, 4 - i, 36, 2 + (i * 2));
			vbl();
			vbl();
		}
	}
	else
		drawWindow(2, 2, 36, 6);
	dialogueBoxIsOpen = 1;

	if (dialoguePortrait)
	{
		MISC->DmaCopy(TILESET + 0x1800, (int8_t*)portraits[dialoguePortrait - 1], 0x80, DMA_INT);
		MISC->DmaCopy(PALETTE + 256 + 256 - 32, (int16_t*)portraits[dialoguePortrait - 1 + 8], 16, DMA_SHORT);
		OBJECTS_A[253] = OBJECTA_BUILD(192, 0, 1, 14);
		OBJECTS_B[253] = OBJECTB_BUILD(3 * 8, 3 * 8, 1, 1, 0, 0, 1, 0);
	}
	else
		OBJECTS_A[253] = OBJECTA_BUILD(5, 0, 0, 14);

	if (*c != 0)
	{
		int x = dialoguePortrait ? 8 : 3;
		int sx = x, sy = 3;
		while (*c != 0)
		{
			vbl();
			vbl();
			if (*c == '\b')
			{
				waitForActionKey();
				c++;
				continue;
			}
			if (*c == '\n')
			{
				if (sy == 5)
				{
					//Already have a full box. Scroll it.
					for (int i = 0; i < 2; i++)
					{
						MISC->DmaCopy(&MAP4[3 * 64], &MAP4[4 * 64], 360, DMA_BYTE);
						MISC->DmaClear(&MAP4[6 * 64] + 3, 0xF301, 34, DMA_SHORT);
						vbl(); vbl(); vbl(); vbl();
					}
					sy = 3;
				}
				sy += 2;
				sx = x;
				c++;
				continue;
			}
			uint16_t t = ((unsigned char)*c - 16) * 2;
			t |= 0xF000;
			MAP4[(sy * 64) + sx +  0] = t;
			MAP4[(sy * 64) + sx + 64] = t + 1;
			sx++;
			c++;
		}
		waitForActionKey();
	}

	if (flags & 1)
	{
		while (INP_KEYMAP[KEY_ACTION]) { vbl(); }
		return;
	}

	dialogueBoxIsOpen = 0;
	OBJECTS_A[253] = OBJECTA_BUILD(5, 0, 0, 14);
	for (int i = 2; i >= 0; --i)
	{
		MISC->DmaClear(MAP4, 0, WIDTH * 8, DMA_INT);
		drawWindow(2, 4 - i, 36, 2 + (i * 2));
		vbl();
		vbl();
	}
	MISC->DmaClear(MAP4, 0, WIDTH * 8, DMA_INT);
	while (INP_KEYMAP[KEY_ACTION]) { vbl(); }
}

int doMenu(int left, int top, char* options, int num)
{
	char *b = options;
	int len = 0;
	for (int i = 0; i < num; i++)
	{
		int nl = strlen(b);
		if (nl > len)
			len = nl;
		b += nl + 1;
	}
	drawWindow(left, top, len + 5, (num * 2) + 2);
	b = options;
	for (int i = 0; i < num; i++)
	{
		drawString(left + 3, top + 1 + (i * 2), b);
		b += strlen(b) + 1;
	}
	MISC->DmaCopy(TILESET + 32, cursorTiles, 48, DMA_INT);

	OBJECTS_A[255] = OBJECTA_BUILD(1, 0, 1, 15);
	OBJECTS_A[254] = OBJECTA_BUILD(5, 1, 1, 15);

	int choice = 0;
	lastInput = 0;
	while (INP_KEYMAP[KEY_ACTION]) { vbl(); }
	while (lastInput != KEY_ACTION)
	{
		OBJECTS_B[255] = OBJECTB_BUILD(((left + 1) * 8) - 4, ((top + 1 + (choice * 2)) * 8) + ((REG_TICKCOUNT / 32) % 2), 0, 0, 0, 0, 1, 0);
		OBJECTS_B[254] = OBJECTB_BUILD(((left + 1) * 8) - 4, (top + 2 + (choice * 2)) * 8, 0, 0, 0, 0, 1, 0);
		vbl();
		getInput();
		switch (lastInput)
		{
			case KEY_UP:
				if (choice == 0) choice = num;
				choice--;
				while (lastInput) { vbl(); getInput(); }
				break;
			case KEY_DOWN:
				choice++;
				if (choice == num) choice = 0;
				while (lastInput) { vbl(); getInput(); }
				break;
		}
	}
	lastInput = 0;
	OBJECTS_A[255] = 0;
	OBJECTS_A[254] = 0;
	eraseWindow(left, top, len + 5, (num * 2) + 2);
	return choice;
}

#define MAXSTACK 250
#define MAXPRINTBUFFER 250

void runScript(uint8_t* code, int entityID)
{
	uint8_t* pc = code;
	uint32_t acc = 0;
	uint32_t stack[MAXSTACK] = { 0 };
	char printBuffer[MAXPRINTBUFFER] = { 0 };
	int stackSize = 0;
	int argc = 0;
	uint8_t cmd;
	while (*pc != 0xFF)
	{
		cmd = *pc++;

		switch(cmd)
		{
			case 0x00: break; //nop
			case 0x01: //add
			{
				argc = stack[--stackSize];
				acc = 0;
				while (argc--)
				{
					acc += stack[--stackSize];
				}
				stack[stackSize++] = acc;
				break;
			}
			case 0x02: //sub
			{
				argc = stack[--stackSize] - 1;
				acc = stack[--stackSize];
				while (argc--)
				{
					acc -= stack[--stackSize];
				}
				stack[stackSize++] = acc;
				break;
			}
			case 0x08: //toss
			{
				acc = stack[--stackSize];
				break;
			}
			case 0x09: //dup
			{
				acc = stack[--stackSize];
				stack[stackSize++] = acc;
				stack[stackSize++] = acc;
				break;
			}
			case 0x0C: //pushvar
			{
				acc = scriptVariables[*pc++];
				stack[stackSize++] = acc;
				break;
			}
			case 0x0D: //pushbyte
			{
				acc = *pc++;
				stack[stackSize++] = acc;
				break;
			}
			case 0x0E: //pushword
			{
				acc = (*pc++ << 8);
				acc |= *pc++;
				stack[stackSize++] = acc;
				break;
			}
			case 0x0F: //pushptr
			{
				acc = (*pc++ << 24);
				acc |= (*pc++ << 16);
				acc |= (*pc++ << 8);
				acc |= (*pc++ << 0);
				stack[stackSize++] = acc;
				break;
			}
			case 0x10: //pushlit#
			case 0x11:
			case 0x12:
			{
				acc = cmd - 0x10;
				stack[stackSize++] = acc;
				break;
			}

			case 0x81: //print
			{
				argc = stack[--stackSize];
				acc = stack[--stackSize];
				char* po = printBuffer;
				char* pi = (char*)acc;
				//pipopapo!
				while (*pi != 0)
				{
					if (*pi == '%')
					{
						pi++;
						switch (*pi)
						{
							case '%': *po++ = '%'; break;
							case 'd': po += TEXT->Format(po, "%d", stack[--stackSize]); argc--; break;
							case 's': po += TEXT->Format(po, "%s", stack[--stackSize]); argc--; break;
							default: break;
						}
						pi++;
					}
					else
						*po++ = *pi++;
				}
				*po = 0;
				saySomething(printBuffer, (argc > 1 && stack[--stackSize]));
				break;
			}
			case 0x82: //face
			{
				argc = stack[--stackSize];
				int d = entityID;
				acc = 4;
				if (argc)
					acc = stack[--stackSize];
				if (acc < 4)
					entities[d].facing = acc;
				else if (acc == 4)
				{
					if (entities[d].x > entities[0].x)
						entities[d].facing = faceLeft;
					else if (entities[d].x < entities[0].x)
						entities[d].facing = faceRight;
					else
					{
						if (entities[d].y > entities[0].y)
							entities[d].facing = faceUp;
						else if (entities[d].y < entities[0].y)
							entities[d].facing = faceDown;
					}
				}
				drawEntity((MapEntity*)&entities[d]);
				break;
			}
			case 0x83: //ask
			{
				argc = stack[--stackSize];
				//sprintf(printBuffer, "pb $%x", printBuffer); saySomething(printBuffer, 0);
				printBuffer[0] = 0;
				char *biga = printBuffer;
				char *chu;
				int opts = 0;
				while (argc--)
				{
					chu = (char*)stack[--stackSize];
					strcpy(biga, chu);
					biga += strlen(biga);
					*biga++ = 0;
					opts++;
				}
				acc = doMenu(2, 8, printBuffer, opts);
				break;
			}
			case 0x84: //portrait
			{
				argc = stack[--stackSize];
				if (argc == 0)
					acc = 0;
				else
				{
					acc = stack[--stackSize];
					argc--;
				}
				while (argc--) --stackSize;
				dialoguePortrait = acc;
				break;
			}
		}
		//printf(" 0x%04X  ", acc);
		//for (int i = 0; i < stackSize; i++)	printf("%d ", stack[i]);
		//printf("\n");
		//if (cmd == 0x06) printf("> \"%s\"\n", printBuffer);
	}
}
