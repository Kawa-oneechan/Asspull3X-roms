#include "../ass.h"
IBios* interface;

extern const TPicFile hdma_api;

int main(void)
{
	REG_SCREENFADE = 31;
	DRAW->DisplayPicture(&hdma_api);
	DRAW->Fade(true, false);
	while(1);
}
