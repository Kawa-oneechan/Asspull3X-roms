#include "../ass.h"
IBios* interface;

#define STDIN  ((FILE*)-1)
#define STDOUT ((FILE*)-2)

static const char sctoasc[] = {
//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	0,   0,   0,   0,   0,   0,   0,   0,   '\b','\t',0,   0,   0,   '\n',0,   0,   // 0x00
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   27,  0,   0,   0,   0,   // 0x10
	' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x20
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 0,   0,   0,   0,   0,   0,   // 0x30
	0,   'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', // 0x40
	'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 0,   0,   0,   0,   0,   // 0x50
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x80
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x90
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xa0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ';', '=', ',', '-', '.', '/', // 0xb0
	'`', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xc0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '[', '\\',']', '\'',0,   // 0xd0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xe0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xf0

//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
	0,   0,   0,   0,   0,   0,   0,   0,   0,   '\t',0,   0,   0,   '\n',0,   0,   // 0x00
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   27,  0,   0,   0,   0,   // 0x10
	' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x20
	')', '!', '@', '#', '$', '%', '^', '^', '&', '*', '(', 0,   0,   0,   0,   0,   // 0x30
	0,   'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', // 0X40
	'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 0,   0,   0,   0,   0,   // 0X50
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x80
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x90
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xa0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   ':', '+', '<', '_', '>', '?', // 0xb0
	'~', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xc0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   '{', '|', '}', '\"',0,   // 0xd0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0xe0
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0    // 0xf0
};

int getc(FILE* stream)
{
	if (stream != STDIN)
	{
		//printf("WARNING: TRIED TO READ FROM A FILE. NOT IMPLEMENTED YET.");
		if (DISK->FileEnd(stream))
		{
			printf("[eof]");
			return -1;
		}
		char buffer[1];
		DISK->ReadFile(stream, buffer, 1);
		return buffer[0];
	}
	unsigned short key = 0;
	while (1)
	{
		key = REG_KEYIN;
		if ((key & 0xFF) > 0)
			break;
	}
	while (1)
	{
		if ((REG_KEYIN & 0xFF) == 0)
			break;
	}
	return sctoasc[key];
}



int32_t main()
{
	interface = (IBios*)(0x01000000);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetCursorPosition(0, 0);
	TEXT->SetTextColor(0, 7);

	char* fuckery = 0;
	int len = 0;
	//while(1)
	//{
	//	getline(&fuckery, &len, STDIN);
	//	printf("> %s\n", fuckery);
	//}
	FILE shit;
	DISK->OpenFile(&shit, "bullshit.txt", FA_READ);
	getline(&fuckery, &len, &shit);
	printf("> %s\n", fuckery);
	DISK->CloseFile(&shit);
	printf("yeah okay.");
	while(1);
}
