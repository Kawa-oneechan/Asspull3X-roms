#include "rpg.h"

BattleOpponent opponents[6];

extern uint8_t canvas[];

void drawBattleUI()
{
	drawWindow(0, 22, 17, 8);
	drawWindow(17, 22, 8, 8);
	drawWindow(25, 22, 15, 8);

	MISC->DmaClear(canvas, 0x88, 0x3C00, DMA_BYTE);

	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 6; i++)
			MAP4[((i + 23) * 64) + (j + 1)] = (32 + j + (32 * i)) | 0xF000;
	for (int j = 0; j < 8; j++)
		for (int i = 0; i < 6; i++)
			MAP4[((i + 23) * 64) + (j + 26)] = (40 + j + (32 * i)) | 0xF000;

	for (int i = 0; i < 6; i++)
	{
		if (party[i].name[0] == 0 || party[i].hpMax == 0)
			continue;
		drawString(party[i].name, 0 + 1, (i * 8) + 1, 1, 0);
		drawString(party[i].name, 0 + 0, (i * 8) + 0, 2, 0);
		drawBar(9, 23 + i, party[i].hp, party[i].hpMax, 5);
	}

	for (int i = 0; i < 6; i++)
	{
		if (opponents[i].monsterID == 0xFFFF)
			continue;
		drawString(opponentDB[opponents[i].monsterID].name, 64 + 1, (i * 8) + 1, 1, 0);
		drawString(opponentDB[opponents[i].monsterID].name, 64 + 0, (i * 8) + 0, 2, 0);
	}

	while(1)
	{
		int action = doMenu(15, 22, 10, -2, "Attack\0Defend\0Item\0Run\0Test\0What?", 6);
		if (action == 0) //attack
		{
			//just for show, switch to the opponent panel

			//leave another cursor on "attack"
			OBJECTS_A[253] = OBJECTA_BUILD(1, 1, 1, 15);
			OBJECTS_B[253] = OBJECTB_BUILD(((15 + 1) * 8) - 4, (23 * 8) - 2, 0, 0, 0, 0, 1, 0);

			//draw "attack" in green
			drawString("Attack", 0 + 0, 96 + 0, 9, 0);

			doMenu(23, 22, 0, -2, "\0\0\0\0\0\0\0\0\0\0", 2);
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

