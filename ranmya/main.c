#include "../ass.h"
IBios* interface;

extern const TImageFile picData;
//extern const char* picData;

#define SPRITEA_BUILD(t,e,p)	\
(								\
	(((p) & 15) << 12) |		\
	(((e) &  1) << 11) |		\
	(((t) & 0x1FF) << 0)		\
)
#define SPRITEB_BUILD(hp,vp,dw,dh,hf,vf,ds,pr)	\
(												\
	(((pr) & 3) << 30) |						\
	(((ds) & 1) << 28) |						\
	(((vf) & 1) << 27) |						\
	(((hf) & 1) << 26) |						\
	(((dh) & 1) << 25) |						\
	(((dw) & 1) << 24) |						\
	(((vp) & 0x7FF) << 12) |				\
	(((hp) & 0x7FF) << 0)					\
)

int main(void)
{
	interface = (IBios*)(0x01000000);
	REG_SCREENFADE = 31;
	DRAW->DisplayPicture((TImageFile*)&picData);

	//SPRITES_A[0] = SPRITEA_BUILD(0 * 16,1,0);
	//SPRITES_B[0] = SPRITEB_BUILD(8, 8, 1,1, 0,0, 1, 0);

	DRAW->FadeFromBlack();
	while(1);
}
