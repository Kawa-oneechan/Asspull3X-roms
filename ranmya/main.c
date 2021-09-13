#include "../ass.h"
IBios* interface;

extern const TImageFile picData;

int main(void)
{
	REG_SCREENFADE = 31;
	DRAW->DisplayPicture((TImageFile*)&picData);
	DRAW->FadeFromBlack();
	while(1);
}
