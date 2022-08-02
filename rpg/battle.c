#include "rpg.h"

BattleOpponent opponents[6];

extern const uint16_t battlebgMap[];
extern const uint16_t battlebgTiles[];
extern const uint16_t battlebgPal[];

static const uint16_t hdma1[] =
{
	0x6018,0x6017,0x6017,0x6016,0x6016,0x6036,0x6035,0x6035,
	0x6035,0x6054,0x6054,0x6054,0x6053,0x6073,0x6073,0x6072,
	0x6072,0x6071,0x6091,0x6091,0x6090,0x6090,0x60B0,0x64AF,
	0x64AF,0x64AF,0x64CE,0x64CE,0x64CD,0x64CD,0x64CD,0x64EC,
	0x64EC,0x64EC,0x64EB,0x650B,0x650B,0x650A,0x650A,0x652A,
	0x6529,0x6529,0x6528,0x6528,0x6548,0x6547,0x6547,0x6947,
	0x6966,0x6966,0x6966,0x6965,0x6985,0x6984,0x6984,0x6984,
	0x69A3,0x69A3,0x69A3,0x69A2,0x69A2,0x69C2,0x69C1,0x69C1,
	0x69C1,0x69E0,0x69E0,0x69E0,0x6E01,0x6E03,0x6E25,0x6E46,
	0x7268,0x7289,0x72AB,0x72AC,0x76CE,0x76F0,0x7711,0x7733,
	0x7B54,0x7B76,0x7B78,0x7B99,0x7FBB,0x7FDC,0x7FFE,0x7FFF,
	0x7FFF,0x7FFE,0x7FDC,0x7FBB,0x7B99,0x7B78,0x7B76,0x7B54,
	0x7733,0x7711,0x76F0,0x76CE,0x72AC,0x72AB,0x7289,0x7268,
	0x6E46,0x6E25,0x6E03,0x6E01,0x69E0,0x69E0,0x69E0,0x69C1,
	0x69C1,0x69C1,0x69C2,0x69A2,0x69A2,0x69A3,0x69A3,0x69A3,
	0x6984,0x6984,0x6984,0x6985,0x6965,0x6966,0x6966,0x6966,
	0x6947,0x6547,0x6547,0x6548,0x6528,0x6528,0x6529,0x6529,
	0x652A,0x650A,0x650A,0x650B,0x650B,0x64EB,0x64EC,0x64EC,
	0x64EC,0x64CD,0x64CD,0x64CD,0x64CE,0x64CE,0x64AF,0x64AF,
	0x64AF,0x60B0,0x6090,0x6090,0x6091,0x6091,0x6071,0x6072,
	0x6072,0x6073,0x6073,0x6053,0x6054,0x6054,0x6054,0x6035,
	0x6035,0x6035,0x6036,0x6016,0x6016,0x6017,0x6017,0x6018,
	0x4820,0x4820,0x4820,0x4420,0x4420,0x4020,0x4020,0x4020,
	0x3C20,0x3C20,0x3820,0x3820,0x3820,0x3420,0x3420,0x3020,
	0x3020,0x3020,0x2C20,0x2C20,0x2820,0x2820,0x2800,0x2400,
	0x2400,0x2000,0x2000,0x2000,0x1C00,0x1C00,0x1800,0x1800,
	0x1800,0x1400,0x1400,0x1000,0x1000,0x1000,0x0C00,0x0C00,
	0x0800,0x0800,0x0800,0x0400,0x0400,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
};
static const uint16_t hdma2[] =
{
	0x69C1,0x69C1,0x69C1,0x65A1,0x65A1,0x61A1,0x61A1,0x61A1,
	0x5D81,0x5D81,0x5981,0x5981,0x5961,0x5561,0x5561,0x5161,
	0x5161,0x4D41,0x4D41,0x4D41,0x4941,0x4921,0x4521,0x4521,
	0x4121,0x4101,0x4101,0x3D01,0x3D01,0x3901,0x38E1,0x34E0,
	0x34E0,0x34E0,0x30C0,0x30C0,0x2CC0,0x2CC0,0x2CC0,0x28A0,
	0x28A0,0x24A0,0x24A0,0x2080,0x2080,0x2080,0x1C80,0x1C60,
	0x1860,0x1860,0x1460,0x1460,0x1440,0x1040,0x1040,0x0C40,
	0x0C20,0x0C20,0x0820,0x0820,0x0400,0x0000,0x0000,0x0000,
};

extern uint8_t canvas[];
extern int dialogueBoxIsOpen, dialoguePortrait;

void drawBattleUI()
{
	MISC->DmaCopy(TILESET + 0x4000, (int8_t*)&battlebgTiles, 2048, DMA_INT);
	MISC->DmaCopy(PALETTE, (int8_t*)&battlebgPal, 16, DMA_SHORT);
	MISC->DmaCopy(MAP1, (int8_t*)&battlebgMap, 2048, DMA_INT);
	MISC->DmaCopy(TILESET + 0xC000, (int8_t*)&uiTiles, 0x100, DMA_INT);

	REG_HDMASOURCE[4] = (int32_t)hdma1;
	REG_HDMATARGET[4] = (int32_t)PALETTE + 2;
	REG_HDMACONTROL[4] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (0 << 8) | (480 << 20);
	REG_HDMASOURCE[5] = (int32_t)hdma2;
	REG_HDMATARGET[5] = (int32_t)PALETTE + 4;
	REG_HDMACONTROL[5] = DMA_ENABLE | HDMA_DOUBLE | (DMA_SHORT << 4) | (352 << 8) | (128 << 20);

//	drawWindow(0, 22, 17, 8);
//	drawWindow(17, 22, 8, 8);
//	drawWindow(25, 22, 15, 8);

	MISC->DmaClear(canvas, 0, 0x3C00, DMA_BYTE);

	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 6; i++)
			MAP4[((i + 23) * 64) + (j + 1)] = (128 + 32 + j + (32 * i)) | 0xF000;
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 6; i++)
			MAP4[((i + 23) * 64) + (j + 26)] = (128 + 40 + j + (32 * i)) | 0xF000;

	for (int i = 0; i < 6; i++)
	{
		if (party[i].name[0] == 0 || party[i].hpMax == 0)
			continue;
		drawString(party[i].name, 0 + 1, 32 + (i * 8) + 1, 1, 0);
		drawString(party[i].name, 0 + 0, 32 + (i * 8) + 0, 2, 0);
		drawBar(9, 23 + i, party[i].hp, party[i].hpMax, 5);
	}

	for (int i = 0; i < 6; i++)
	{
		if (opponents[i].monsterID == 0xFFFF)
			continue;
		drawString(opponentDB[opponents[i].monsterID].name, 64 + 1, 32 + (i * 8) + 1, 1, 0);
		drawString(opponentDB[opponents[i].monsterID].name, 64 + 0, 32 + (i * 8) + 0, 2, 0);
	}

	dialoguePortrait = 3;
	saySomething("It  is  I  who   added the\nnuts to that!");

	while(1)
	{
		int action = doMenu(15, 22, -2, -2, "Attack\0Defend\0Item\0Run\0Test\0What?", 6);
		if (action == 0) //attack
		{
			//just for show, switch to the opponent panel

			//leave another cursor on "attack"
			OBJECTS_A[253] = OBJECTA_BUILD(1, 1, 1, 15);
			OBJECTS_B[253] = OBJECTB_BUILD(((15 + 1) * 8) - 4, (23 * 8) - 2, 0, 0, 0, 0, 1, 0);

			//draw "attack" in green
			drawString("Attack", 0 + 0, 96 + 0, 9, 0);

			doMenu(23, 22, -2, -2, "\0\0\0\0\0\0\0\0\0\0", 2);
			//ignore the choice for now

			//hide the shadow cursor
			OBJECTS_A[253] = 0;
		}
	}
}

void startBattle(int formation)
{
	for (int i = 0; i < 6; i++)
	{
		if (formationsDB[formation][i] == 0)
		{
			opponents[i].bits = -1;
			opponents[i].monsterID = -1;
		}
		else
		{
			const int monsterID = formationsDB[formation][i] - 1;
			const BattleOpponentDB* opDB = &opponentDB[monsterID];
			opponents[i].monsterID = monsterID;
			opponents[i].bits = opDB->bits;
			opponents[i].hp = opponents[i].hpMax = opDB->hp;
			opponents[i].pp = opponents[i].ppMax = opDB->pp;
			if (opDB->handler != NULL)
				opDB->handler((void*)opDB, &opponents[i], 0);
		}
	}
	drawBattleUI();
}

