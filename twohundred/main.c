#include "../ass.h"
IBios* interface;

extern const TImageFile _200_16;

int main(void)
{
//	REG_SCREENFADE = 31;
	DRAW->DisplayPicture((TImageFile*)&_200_16);
//	REG_SCREENMODE |= SMODE_BOLD;
//	DRAW->FadeFromBlack();
	REG_SCREENFADE = 0;
	while(1);
}
