#include "../ass.h"
IBios* interface;

extern const TImageFile picData;

int main(void)
{
	interface = (IBios*)(0x01000000);
//	REG_SCREENFADE = 31;
	DRAW->DisplayPicture((TImageFile*)&picData);
//	REG_SCREENMODE |= SMODE_BOLD;
//	DRAW->FadeFromBlack();
	REG_SCREENFADE = 0;
	while(1);
}
