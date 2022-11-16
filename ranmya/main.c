#include "../ass.h"
IBios* interface;

extern const TPicFile ranmya;

int main(void)
{
	REG_SCREENFADE = 31;
	DRAW->DisplayPicture(&ranmya);
	DRAW->Fade(true, false);
	while(1);
}
