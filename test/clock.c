#include "../ass.h"
#include "../ass-keys.h"

extern char* asctime(const tm*);
extern tm* gmtime(const time_t*);
extern time_t mktime(tm*);

extern void WaitForKey();
void Spinner()
{
	static int spin = 0;
	const char prompt[] = "Press any key to continue.";
	const char spinner[] = "\xB8\x08+*+\x08\xB8    ";

	if (spin == 0)
	{
		for (int i = 0; i < 26; i++)
			TEXTMAP[(29 * 80) + i] = 0x07 | (prompt[i] << 8);
	}

	TEXTMAP[(29 * 80) + 29] = 0x0E | (spinner[((spin++) >> 4) % 11] << 8);
	TEXTMAP[(29 * 80) + 28] = 0x0E | (spinner[((spin >> 4) + 1) % 11] << 8);
	TEXTMAP[(29 * 80) + 27] = 0x0E | (spinner[((spin >> 4) + 2) % 11] << 8);
	vbl();
}

void DrawClock(tm* now_tm)
{
	TEXT->SetCursorPosition(4, 4);
	printf("\x93\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8E\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8B");
	TEXT->SetCursorPosition(4, 5);
	printf("\x89 %04d-%02d-%02d \x89 %02d:%02d:%02d \x89", now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
	TEXT->SetCursorPosition(4, 6);
	printf("\x8C\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8D\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x92");
}


const int FontWH_Ratio = 2;
const int cosa[] = { 0, 105, 208, 309, 407, 500, 588, 669, 743, 809, 866, 914, 951, 978, 995, 1000, 995, 978, 951, 914, 866, 809, 743, 669, 588, 500, 407, 309, 208, 105, 0, -105, -208, -309, -407, -500, -588, -669, -743, -809, -866, -914, -951, -978, -995, -1000, -995, -978, -951, -914, -866, -809, -743, -669, -588, -500, -407, -309, -208, -105 };
const int sina[] = { -1000, -995, -978, -951, -914, -866, -809, -743, -669, -588, -500, -407, -309, -208, -105, 0, 105, 208, 309, 407, 500, 588, 669, 743, 809, 866, 914, 951, 978, 995, 1000, 995, 978, 951, 914, 866, 809, 743, 669, 588, 500, 407, 309, 208, 105, 0, -105, -208, -309, -407, -500, -588, -669, -743, -809, -866, -914, -951, -978, -995 };

static void drawChar(int x, int y, int c)
{
	TEXTMAP[(y * 80) + x] = c;
}

static void drawHand(int minute, int hlength, int c, int sYcen, int sXcen)
{
	int x, y, n;
	for (n = 1; n < hlength; n++)
	{
		x = (cosa[minute] * n * FontWH_Ratio / 1000) + sXcen;
		y = (sina[minute] * n / 1000) + sYcen;
		drawChar(x, y, c);
	}
}

static void drawCircle(int hand_max, int sYcen, int sXcen)
{
	int x, y, r;
	int c;

	for (r = 0; r < 60; r++)
	{
		x = (cosa[r] * hand_max * FontWH_Ratio / 1000) + sXcen;
		y = (sina[r] * hand_max / 1000) + sYcen;
		switch (r)
		{
			case 0:
			case 5:
			case 10:
			case 15:
			case 20:
			case 25:
			case 30:
			case 35:
			case 40:
			case 45:
			case 50:
			case 55:
				c = 0x880F;
				break;
			default:
				c = 0x0807;
				break;
		}
		drawChar(x, y, c);
	}
}


void ClockTest()
{
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	printf("Real-time clock test");
	TEXT->SetCursorPosition(0, 2);

	time_t now_t = REG_TIMET;
	tm* now_tm = gmtime(&now_t);

	REG_CARET = 0;

	const int hand_max = 10;
	const int sXcen = 54;
	const int sYcen = 16;

	drawCircle(hand_max, sYcen, sXcen);

	while(INP_KEYIN == 0)
	{
		if (now_t != REG_TIMET)
		{
			drawHand(now_tm->tm_hour % 12 * 5 + now_tm->tm_min / 12, 2 * hand_max / 3, 0, sYcen, sXcen);
			drawHand(now_tm->tm_min, hand_max - 2, 0, sYcen, sXcen);
			drawHand(now_tm->tm_sec, hand_max - 1, 0, sYcen, sXcen);
		}

		now_t = REG_TIMET;
		now_tm = gmtime(&now_t);

		drawHand(now_tm->tm_hour % 12 * 5 + now_tm->tm_min / 12, hand_max - 3, 0x8809, sYcen, sXcen);
		drawHand(now_tm->tm_min, hand_max - 2, 0x880A, sYcen, sXcen);
		drawHand(now_tm->tm_sec, hand_max - 1, 0x880B, sYcen, sXcen);

		DrawClock(now_tm);

		TEXT->SetCursorPosition(0, 8);
		printf("Time_T: %llu\nAscTime: \"%s\"\n", now_t, asctime(now_tm));
		printf("GMTime:\n  sec   %d\n  min   %d\n  hour  %d\n  mday  %d\n  mon   %d\n  year  %d\n  wday  %d\n  yday  %d\n", now_tm->tm_sec, now_tm->tm_min, now_tm->tm_hour, now_tm->tm_mday, now_tm->tm_mon, now_tm->tm_year, now_tm->tm_wday, now_tm->tm_yday);

		Spinner();
	}
}