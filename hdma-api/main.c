#include "../ass.h"
IBios* interface;

extern const TImageFile hdma_api;

int main(void)
{
	REG_SCREENFADE = 31;
	DRAW->DisplayPicture((TImageFile*)&hdma_api);
	DRAW->Fade(true, false);
	while(1);
}
