#include "../ass.h"
IBios* interface;

extern const TPicFile _200_16;

int main(void)
{
//	REG_SCREENFADE = 31;
	DRAW->DisplayPicture(&_200_16);
//	REG_SCREENMODE |= SMODE_BOLD;
//	DRAW->Fade(true, false);
	REG_SCREENFADE = 0;
	while(1);
}
