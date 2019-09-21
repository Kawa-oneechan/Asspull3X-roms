#include "../ass.h"

#ifdef printf
#undef printf
#define printf Write
#endif

#include "funcs.h"

int inVblank;

//from crt0.s
extern IBios interface;

extern int32_t cursorPos;
extern int8_t attribs;
extern int8_t textWidth, textHeight;

extern void cartStart();
extern IBios interface;

#define logoHeight 4
const uint16_t logo[] = {
	0x2007,0x2007,0x2007,0x410b,0xb80b,0x530b,0xb80b,
	0x530b,0xb80b,0x500b,0xb80b,0x550b,0xb80b,0x4c0b,
	0xb80b,0x4c0b,0x2007,0x2007,0x2007,0x2007,0x2007,
	0x2007,0x2007,0x2007,0x2007,0x8790,0x8790,0x8790,
	0x8790,0x8790,0x8790,0x8790,0x8790,0x8790,0x8790,
	0x8790,0x8790,0x8790,0x8790,0x2007,0x2007,0x2007,
	0x2007,0x2007,0x2007,0x2007,0x2007,0x2007,0x2007,
	0x8701,0x2090,0x2090,0x2010,0x8701,0x2090,0x2090,
	0x2010,0x8701,0x2090,0x2090,0x2010,0x8701,0x8701,
	0x2007,0x2007,0x2007,0x2007,0x8407,0x8808,0x2007,
	0x2007,0x2007,0x2090,0x2090,0x2010,0x2007,0x2090,
	0x2090,0x2010,0x2007,0x2090,0x2090,0x2010,0x870f,
	0x830f,0x840f,0x2007,0x2007,0x840f,0x870f,0x2007,
	0x2007,0x2007,0x2007,0x2090,0x2090,0x2010,0x2007,
	0x2090,0x2090,0x2010,0x2007,0x2090,0x2090,0x2010,
	0x2007,0x2007,0x2007,0x870f,0x830f,0x830f,0x2007,
	0x2007,0x2007,0x2007,0x8709,0x8719,0x8719,0x8719,
	0x8719,0x8719,0x8719,0x8719,0x8719,0x8719,0x8719,
	0x8719,0x8719,0x8719,0x8710,0x840f,0x870f,0x2007,
	0x870f,0x830f,0x840f,0x2007,0x2007,0x2007,0x4b00,
	0x6100,0x7700,0x6100,0x2000,0x3200,0x3000,0x3100,
	0x3700,0x2007,0x2007,0x2007,0x8408,0x8707,0x2007,
	0x2007,0x2007,0x2007,0x2007,0x8707,0x8808,0x2007
};

void Logo()
{
	int32_t start = (logoHeight * 80) + 28;
	int16_t* target = (int16_t*)MEM_VRAM + start;
	int32_t line = 0;
	int32_t col = 0;
	int16_t* source = (int16_t*)logo;
	for (line = 0; line < 7; line++)
	{
		for (col = 0; col < 23; col++)
		{
			target[0] = source[0];
			target++;
			source++;
		}
		target += 80-23;
	}
}

/*void FadeOut()
{
	int32_t i;
	WaitForVBlanks(5);
	for (i = 0; i < 8; i++)
	{
		PALETTE[i] = 0;
		PALETTE[i+8] = palette[i];
	}
	WaitForVBlanks(5);
	for (i = 0; i < 16; i++)
	{
		PALETTE[i] = 0;
	}
	WaitForVBlanks(5);
}*/

/*
int32_t noiseTicks = 0;
void noise()
{
	if (noiseTicks == 0)
	{
		REG_MIDIOUT = 0xB1 | (  7 << 8) | (110 << 16); //Volume
		REG_MIDIOUT = 0xB1 | ( 91 << 8) | (127 << 16); //Reverb
		REG_MIDIOUT = 0xB1 | ( 93 << 8) | (127 << 16); //Chorus
		REG_MIDIOUT = 0xC1 | ( 48 << 8) | (  0 << 16); //Program: String Ensemble
		REG_MIDIOUT = 0x91 | ( 60 << 8) | ( 80 << 16); //C4
		REG_MIDIOUT = 0x91 | ( 29 << 8) | ( 80 << 16); //F1
		REG_MIDIOUT = 0x91 | ( 41 << 8) | ( 80 << 16); //F2
		REG_MIDIOUT = 0xB2 | (  7 << 8) | ( 89 << 16); //Volume
		REG_MIDIOUT = 0xB2 | ( 91 << 8) | ( 93 << 16); //Reverb
		REG_MIDIOUT = 0xB2 | ( 93 << 8) | (  0 << 16); //Chorus
		REG_MIDIOUT = 0xC2 | ( 32 << 8) | (  0 << 16); //Program: Acoustic Bass
		REG_MIDIOUT = 0x92 | ( 60 << 8) | ( 80 << 16); //C4
		REG_MIDIOUT = 0x92 | ( 29 << 8) | ( 80 << 16); //F1
		REG_MIDIOUT = 0x92 | ( 41 << 8) | ( 80 << 16); //F2
	}
	else if (noiseTicks > 37 && noiseTicks < 110)
	{
		int32_t vol = noiseTicks - 37;
		REG_MIDIOUT = 0xB1 | (7 << 8) | ((110 - vol) << 16);
		REG_MIDIOUT = 0xB2 | (7 << 8) | ((89 - vol) << 16);
	}
	else if (noiseTicks == 110)
	{
		REG_MIDIOUT = 0x81 | ( 60 << 8) | (  0 << 16); //C4
		REG_MIDIOUT = 0x81 | ( 29 << 8) | (  0 << 16); //F1
		REG_MIDIOUT = 0x81 | ( 41 << 8) | (  0 << 16); //F2
		REG_MIDIOUT = 0x82 | ( 60 << 8) | (  0 << 16); //C4
		REG_MIDIOUT = 0x82 | ( 29 << 8) | (  0 << 16); //F1
		REG_MIDIOUT = 0x82 | ( 41 << 8) | (  0 << 16); //F2
		interface.VBlank = 0;
	}
	//PALETTE[0] = (int16_t)noiseTicks;
	noiseTicks++;
}
*/

#define SPRITEA_BUILD(t,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((t) & 0x1FF) << 0)		\
)
#define SPRITEB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
(												\
	(((pr) & 3) << 30) |						\
	(((ds) & 1) << 28) |						\
	(((vf) & 1) << 27) |						\
	(((hf) & 1) << 26) |						\
	(((dh) & 1) << 25) |						\
	(((dw) & 1) << 24) |						\
	(((vp) & 0x7FF) << 12) |				\
	(((hp) & 0x7FF) << 0)					\
)

extern const uint16_t chelsieTiles[2048];
int32_t chelsiePos, chelsieFrame, chelsieTimer;
#define chelsieHeight 230
void SetupChelsie()
{
	// 0 - 0x13000000
	//-1 - 0x130003FF
	chelsiePos = -32;
	chelsieFrame = 0;
	chelsieTimer = 0;
	DmaCopy((int8_t*)0x0E080000, (int8_t*)&chelsieTiles, 2048, DMA_INT);
	//RleUnpack((void*)0x0E080000, (void*)&chelsieTiles);

	*(uint16_t*)0xE108000 = SPRITEA_BUILD(0,1,0);
	*(uint32_t*)0xE108200 = SPRITEB_BUILD(chelsiePos, chelsieHeight, 1,1, 0,0, 1, 0);
	//*(uint32_t*)0xE108200 = SPRITEB_BUILD(64, 64, 0,1, 0,0, 1, 0);
}

void UpdateChelsie()
{
	chelsieTimer++;
	if (chelsieTimer == 16) chelsieTimer = 0;
	chelsieFrame = chelsieTimer / 2;
	chelsiePos += 2;
	if (chelsiePos > 640) chelsiePos = -32;
	*(uint16_t*)0xE108000 = SPRITEA_BUILD(chelsieFrame * 16,1,0);
	*(uint32_t*)0xE108200 = SPRITEB_BUILD(chelsiePos, chelsieHeight, 1,1, 0,0, 1, 0);
	//REG_SCREENFADE = 0;
	//PALETTE[0] = chelsieTimer;
}

void TriggerTest()
{
	PALETTE[14] = 0x1234;
	MIDI_PROGRAM(2, MIDI_PAD1NEWAGE);
	MIDI_KEYON(2, MIDI_C4, 80);
	REG_SCREENFADE = 32;
}

void HideChelsie()
{
	*(uint16_t*)0xE108000 = 0;
}

extern const uint16_t hdma1[], hdma2[];
extern const uint16_t fontTiles[];
#define insertHeight 12

int32_t main(void)
{
	//REG_SCREENMODE = SMODE_TEXT | SMODE_240;
	WaitForVBlank();
	SetTextMode(SMODE_240 | SMODE_BOLD | SMODE_SPRITES);
	DmaCopy((int8_t*)0x0E100200, (int8_t*)&fontTiles, 12288, DMA_INT);
	//REG_SCREENMODE = SMODE_TEXT | SMODE_SPRITES | SMODE_240;
	//SetBold(1);
	//EnableSprites(1);
	cursorPos = 0;
	attribs = 0x07;
	ClearScreen();
	REG_SCREENFADE = 0;

//	WriteFormat("Hah-%s-ha, 100%% joke, %d/7. [%+*d] [%2d] %#4x!\n", "fucking", 5, 4, 32, 7, 0xF00D);
//	WriteFormat("Okay: [%-6s] [%6s]\n", "testicle", "testicle");

/*	cursorPos = 0;
	attribs = 0x17;
	WriteStr("Hello,");
	attribs = 0x1E;
	WriteStr(" world!\n");
*/
	ResetPalette();
	//void(*entry)(void)= (void*)0x00020004;
	//entry();

	SetupChelsie();
	REG_INTRMODE = IMODE_VBLANK; // | IMODE_TRIGGER;
	interface.VBlank = UpdateChelsie;
	//interface.TriggerLine = TriggerTest;
	//REG_TRIGGERLINE = 64;

	REG_HDMASOURCE[0] = (int32_t)hdma1;
	REG_HDMATARGET[0] = (int32_t)PALETTE;
	REG_HDMACONTROL[0] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);

	dpf("BIOS v%d.%d\n", (interface.biosVersion >> 8) & 0xFF, (interface.biosVersion >> 0) & 0xFF);
	Write("BIOS v%d.%d\n", (interface.biosVersion >> 8) & 0xFF, (interface.biosVersion >> 0) & 0xFF);
	//REG_SCREENFADE = 0x80 | 31;
	Logo();
	MIDI_PROGRAM(1, MIDI_SEASHORE);
	MIDI_KEYON(1, MIDI_C4, 80);

	int32_t* cartCode = (int32_t*)0x00020000;
	void(*entry)(void)= (void*)0x00020004;
	char* cartName = (char*)0x00020008;
	attribs = 0x0F;
	cursorPos = (insertHeight * 80) + 40 - (16 / 2);
	printf("* INSERT  CART *");
	int32_t haveDisk;
	while(1)
	{
		//((int32_t*)MEM_VRAM)[0] = *cartCode;
		if (*cartCode != 0x41535321) //ASS!
		{
			haveDisk = *(volatile uint8_t*)(0x0D800032) & 1;
			if (haveDisk)
			{
				//We got a disk!
				DIR dir;
				FILEINFO info;
				FILE file;
				//cursorPos = 0;
				int32_t ret = FindFirst(&dir, &info, "0:/", "start.app");
				//Write("disk:FindFirst -> %d, %s\n", ret, info.fname);
				CloseDir(&dir);
				if (ret == 0 && info.fname[0])
				{
					ret = OpenFile(&file, "0:/start.app", FA_READ);
					if (ret)
					{
						printf("Not a boot disk.");
						continue;
					}
					ret = ReadFile(&file, (void*)0x01002000, info.fsize);
					if (ret < 0)
					{
						printf("Failed to read. ");
						continue;
					}
					//Write("disk:ReadFile -> %d\n", ret);
					ret = CloseFile(&file);
					entry = (void*)0x01002020;
					cartName = (char*)0x01002008;
					goto loadIt;
				}
			}

			//WaitForVBlank();
			//UpdateChelsie();
			//noise();
			continue;
		}
		else
		{
			entry = (void*)0x00020004;
			attribs = 0x0F;
			cursorPos = (insertHeight * 80) + 40 - (16 / 2);
			printf("* INSERT  CART *");
		}


loadIt:
		//Write("BREAKOUT!\n");
		MidiReset();

		//REG_MIDIOUT = 0xC1 | (120 << 8) | (  0 << 16); //Program: Guitar Fret Noise
		//REG_MIDIOUT = 0x91 | ( 60 << 8) | ( 80 << 16); //C4
		MIDI_PROGRAM(1, MIDI_GUITARFRETNOISE);
		MIDI_KEYON(1, MIDI_C4, 80);

		cursorPos = (insertHeight * 80) + 40 - 16;
		for (int i = 0; i < 32; i++)
			((int16_t*)MEM_VRAM)[cursorPos++] = 0x0000;
		int32_t nameLen = strnlen_s(cartName, 128);
		cursorPos = (insertHeight * 80) + 40 - (nameLen / 2); // - 8;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8008;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8108;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8208;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x2087;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8087;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8187;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8287;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x207F;
		attribs = 0x0E;
		printf("%s", cartName);
		//while(*cartName)
		//	((int16_t*)MEM_VRAM)[cursorPos++] = (*cartName++ << 8) | 0x7F;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x207F;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8287;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8187;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8087;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x2087;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8208;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8108;
		//((int16_t*)MEM_VRAM)[cursorPos++] = 0x8008;
		//nameLen = 1000;
		//while(nameLen--);
		//WaitForVBlanks(50);
		FadeToBlack();
		HideChelsie();
		REG_HDMACONTROL[0] = 0;
		interface.VBlank = 0;
		interface.TriggerLine = 0;
		attribs = 0x0F;
		ClearScreen();
		ResetPalette();
		WaitForVBlank();
		REG_SCREENFADE = 0;
		entry();
	}
}

void ExHandler()
{
	attribs = 0x4E;
	cursorPos = 0;
	printf("Fucked up, yo.");
	while(1) WaitForVBlank();
	asm("rte");
}

void AddressHandler()
{
	attribs = 0x4E;
	cursorPos = 0;
	printf("Address error!");
	while(1) WaitForVBlank();
	asm("rte");
}

void InstructionHandler()
{
	attribs = 0x4E;
	cursorPos = 0;
	printf("Illegal mangrasp!");
	while(1) WaitForVBlank();
	asm("rte");
}

void ZeroHandler()
{
	attribs = 0x4E;
	cursorPos = 0;
	printf("Division by zero. Wow.");
	while(1) WaitForVBlank();
	asm("rte");
}

void VBlankHandler()
{
	if (interface.VBlank != 0)
		interface.VBlank();
	asm("rte");
}

void TriggerLineHandler()
{
	if (interface.TriggerLine)
		interface.TriggerLine();
	asm("rte");
}
