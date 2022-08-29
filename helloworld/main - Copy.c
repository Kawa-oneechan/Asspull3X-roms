#include "../ass.h"
IBios* interface;


typedef struct
{
	char code[6];
	char wday_name[7][4];
	char mon_name[12][4];
	char wday_nameF[7][10];
	char mon_nameF[12][10];
	char shortDateFmt[16];
	char longDateFmt[16];
	char timeFmt[16];
	char thousands, decimals;
	char currency[4];
	bool currencyAfter;
	char sctoasc[256];
} TIntloc;


TIntloc DefaultUSLocale =
{
	"en_US",
	{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" },
	{ "Jan", "Feb", "Mar", "Apr", "May", "Jun",
	  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" },
	{ "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" },
	{ "January", "February", "March", "April", "May", "June",
	  "July", "August", "September", "October", "November", "December" },
	"%Y-%m-%d",
	"%A, %B %d, %Y",
	"%H:%M:%S",
	',', '.',
	"$", false,
	{
	//Unshifted
	//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
		0,   '\e','1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b','\t',// 0x00
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',0,   'a', 's', // 0x10
		'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`', 0,   '\\','z', 'x', 'c', 'v', // 0x20
		'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   // 0x30
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40
		'2', '3', '0', '.', 0,   0, '\\',  0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x50
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	//Shifted
	//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
		0,   '\e','!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b','\t',// 0x00
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',0,   'A', 'S', // 0x10
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"','~', 0,   '|', 'Z', 'X', 'C', 'V', // 0x20
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   // 0x30
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40
		'2', '3', '0', '.', 0,   0,  '|',  0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x50
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	}
};

TIntloc DutchLocale =
{
	"nl_NL",
	{ "zo", "ma", "di", "wo", "do", "vr", "za" },
	{ "jan", "feb", "mrt", "apr", "mei", "jun",
	  "jul", "aug", "sep", "okt", "nov", "dec" },
	{ "zondag", "maandag", "dinsdag", "woensdag", "donderdag", "vrijdag", "zaterdag" },
	{ "januari", "februari", "maart", "april", "mei", "juni",
	  "juli", "august", "september", "oktober", "november", "december" },
	"%Y-%m-%d",
	"%A, %d %B, %Y",
	"%H:%M:%S",
	'.', ',',
	"0xB6", false,
	{
	//Unshifted
	//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
		0,   '\e','1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b','\t',// 0x00
		'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',0,   'a', 's', // 0x10
		'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'','`', 0,   '\\','z', 'x', 'c', 'v', // 0x20
		'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   // 0x30
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40
		'2', '3', '0', '.', 0,   0, '\\',  0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x50
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	//Shifted
	//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
		0,   '\e','!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b','\t',// 0x00
		'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',0,   'A', 'S', // 0x10
		'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"','~', 0,   '|', 'Z', 'X', 'C', 'V', // 0x20
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   // 0x30
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40
		'2', '3', '0', '.', 0,   0,  '|',  0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x50
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	}
};

TIntloc HungarianLocale =
{
	"hu_HU",
	{ "V", "H", "K", "Sze", "Cs", "P", "Szo" },
	{ "jan", "febr", "márc", "ápr", "máj", "jún",
	  "júl", "aug", "szep", "okt", "nov", "dec" },
	{ "vasárnap", "hétf\xF5", "kedd", "szerda", "csütörtök", "péntek", "szombat" },
	{ "január", "február", "március", "április", "május", "június",
	  "július", "augusztus", "szeptember", "október", "november", "december" },
	"%Y-%m-%d",
	"%A, %d %B, %Y",
	"%H:%M:%S",
	' ', ',',
	"Ft", false,
	{
	//Unshifted
	//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
		0,   '\e','1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b','\t',// 0x00
		'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p',0xF5, 'ú', '\n',0,   'a', 's', // 0x10
		'd', 'f', 'g', 'h', 'j', 'k', 'l', 'é', 'á',0xFB, 0,   '\\','y', 'x', 'c', 'v', // 0x20
		'b', 'n', 'm', ',', '.', '/', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   // 0x30
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40
		'2', '3', '0', '.', 0,   0,  'í',  0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x50
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	//Shifted
	//  0    1    2    3    4    5    6    7    8    9    a    b    c    d    e    f
		0,   '\e','!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b','\t',// 0x00
		'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P',0xD5, 'Ú', '\n',0,   'A', 'S', // 0x10
		'D', 'F', 'G', 'H', 'J', 'K', 'L', 'É', 'Á',0xDB, 0,   '|', 'Y', 'X', 'C', 'V', // 0x20
		'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   // 0x30
		0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', '-', '4', '5', '6', '+', '1', // 0x40
		'2', '3', '0', '.', 0,   0,  'Í',  0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x50
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x60
		0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   // 0x70
	}
};



int main(void)
{
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	TEXT->SetTextColor(1, 7);
	TEXT->ClearScreen();
	printf("Hello, world!\n");
}
