#include "../ass.h"
#include "../ass-std.h"

IBios* interface;

static const char sctoasc[256] = {
	0,0,'1','2','3','4','5','6','7','8','9','0','-','=',0,0,
	'q','w','e','r','t','y','u','i','o','p','[',']',0,0,'a','s',
	'd','f','g','h','j','k','l',';',39,'`',0,92,'z','x','c','v',
	'b','n','m',',','.','/',0,'*',0,32,0,0,0,0,0,0,
	0,0,0,0,0,0,0,'7','8','9','-','4','5','6','+','1',
	'2','3','0','.',0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

time_t tictoc = 0;

void timer(){
	int oldposition=REG_CARET;
	//Set Cursor Position
	TEXT->SetCursorPosition(16, 1);
	printf("%llu", REG_TIMET-tictoc);
	REG_CARET=oldposition;
}

char getchr()
{
	uint16_t key = 0;
	while (1)
	{
		vbl();
		key = INP_KEYIN;
		if ((key & 0xFF) > 0)
			break;
	}
	return sctoasc[key & 0xFF];
}

int main(void)
{
	//setting default colours
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();
	printf("Felin for hire test line\n");
	printf("You got blue (1) background and gray (7) text as two of the 16 colours.\n");

	int points=0;

	//setting the VBlank pointer to the timer
	interface->VBlank=timer;

	inton();

	//Print line by pressing
	//X to exit, otherwise loop on
	while(1){
		tictoc=REG_TIMET;

		TEXT->ClearScreen();
		printf("Felin for hire test line\n");
		printf("Points: %d Time:  \n", points);
		printf("Press a to increase points by one, b to increase points by ten.\n");
		printf("Press r to reset, x to exit, press any key to fail:\n");
		char v = getchr();

		//v is the character we get, you can check what it is
		if (v=='a'){
			points++;
		}
		if (v=='b' && REG_TIMET>=tictoc+10) {
			points+=10;
		}
		if(v=='r'){
			printf("Reset. \n");
			points=0;
		}
		if (v=='x'){
			printf("Thanks for playing the pointless points!");
			break;
		}
	}
}
