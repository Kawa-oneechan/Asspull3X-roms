#include "../ass.h"
IBios* interface;

extern const TImageFile ranmya;

int main(void)
{
	REG_SCREENFADE = 31;
	DRAW->DisplayPicture((TImageFile*)&ranmya);
	DRAW->Fade(true, false);
	while(1);
}
