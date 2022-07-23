#include "rpg.h"

bool tryToFreeze(BattleParticipant* target)
{
	if (GETFACTION(target) == factSunyellow)
		return false;
	if (target->bits & BIT_CANTFREEZE)
		return false;
	target->bits |= BIT_FROZEN;
	return true;
}

void swapPartyMembers(int a, int b)
{
	PartyMember limbo;
	memcpy(&limbo, &party[a], sizeof(PartyMember));
	memcpy(&party[a], &party[b], sizeof(PartyMember));
	memcpy(&party[b], &limbo, sizeof(PartyMember));
}
