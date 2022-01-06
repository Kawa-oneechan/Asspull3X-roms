#include "../ass.h"
extern IBios* interface;

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

#define KEY_UP 0xC8
#define KEY_LEFT 0xCB
#define KEY_RIGHT 0xCD
#define KEY_DOWN 0xD0
#define KEY_ENTER 0x1C

void DrawClock(tm* now_tm)
{
	TEXT->SetCursorPosition(4, 4);
	printf("\x93\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8E\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8B");
	TEXT->SetCursorPosition(4, 5);
	printf("\x89 %04d-%02d-%02d \x89 %02d:%02d:%02d \x89", now_tm->tm_year + 1900, now_tm->tm_mon + 1, now_tm->tm_mday, now_tm->tm_hour, now_tm->tm_min, now_tm->tm_sec);
	TEXT->SetCursorPosition(4, 6);
	printf("\x8C\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x8D\x90\x90\x90\x90\x90\x90\x90\x90\x90\x90\x92");
}

void ClockTest()
{
reset:
	TEXT->SetTextColor(0, 7);
	TEXT->ClearScreen();
	printf("Real-time clock test");
	TEXT->SetCursorPosition(0, 2);

	time_t now_t = REG_TIMET;
	tm* now_tm = gmtime(&now_t);

	if (now_t == 0)
	{
		printf("RTC isn't set. Please enter the correct time:");

		const int muls[] = { 1000, 100, 10, 1 };
		int cursor = 0;
		int changed = 1;
		REG_CARET = 0xC000; //block shape please
		while (1)
		{
			if (changed)
			{
				DrawClock(now_tm);
				changed = 0;
			}

			TEXT->SetCursorPosition(6 + cursor, 5);

			int in = REG_KEYIN;
			if (in == KEY_ENTER)
			{
				REG_TIMET = mktime(now_tm);
				break;
			}
			else if (in == KEY_LEFT)
			{
				if (cursor == 0) cursor = 20;
				else if (cursor == 5) cursor = 3;
				else if (cursor == 8) cursor = 6;
				else if (cursor == 13) cursor = 9;
				else if (cursor == 16) cursor = 14;
				else if (cursor == 19) cursor = 17;
				else cursor--;
			}
			else if (in == KEY_RIGHT)
			{
				if (cursor == 20) cursor = 0;
				else if (cursor == 3) cursor = 5;
				else if (cursor == 6) cursor = 8;
				else if (cursor == 9) cursor = 13;
				else if (cursor == 11) cursor = 14;
				else if (cursor == 14) cursor = 16;
				else if (cursor == 17) cursor = 19;
				else cursor++;
			}
			else if (in == KEY_UP || in == KEY_DOWN)
			{
				switch(cursor)
				{
					//year
					case 0:
					case 1:
					case 2:
					case 3:
					{
						int y = now_tm->tm_year + 1900;
						int mul = muls[cursor];
						if (in == KEY_DOWN) mul = -mul;
						y += mul;
						now_tm->tm_year = y - 1900;
						changed = 1;
						break;
					}
					//month
					case 5:
					case 6:
					{
						int y = now_tm->tm_mon + 1;
						int mul = muls[cursor - 3];
						if (in == KEY_DOWN) mul = -mul;
						y += mul;
						if (y > 12) y = 1;
						else if (y <= 0) y = 12;
						now_tm->tm_mon = y - 1;
						changed = 1;
						break;
					}
					//day
					case 8:
					case 9:
					{
						int y = now_tm->tm_mday;
						int mul = muls[cursor - 6];
						if (in == KEY_DOWN) mul = -mul;
						y += mul;
						if (y > 31) y = 1;
						else if (y <= 0) y = 31;
						now_tm->tm_mday = y;
						changed = 1;
						break;
					}
					//hour
					case 13:
					case 14:
					{
						int y = now_tm->tm_hour;
						int mul = muls[cursor - 11];
						if (in == KEY_DOWN) mul = -mul;
						y += mul;
						if (y > 23) y = 0;
						else if (y <= 0) y = 23;
						now_tm->tm_hour = y;
						changed = 1;
						break;
					}
					//minute
					case 16:
					case 17:
					{
						int y = now_tm->tm_min;
						int mul = muls[cursor - 14];
						if (in == KEY_DOWN) mul = -mul;
						y += mul;
						if (y > 59) y = 0;
						else if (y <= 0) y = 59;
						now_tm->tm_min = y;
						changed = 1;
						break;
					}
					//second
					case 19:
					case 20:
					{
						int y = now_tm->tm_sec;
						int mul = muls[cursor - 17];
						if (in == KEY_DOWN) mul = -mul;
						y += mul;
						if (y > 59) y = 0;
						else if (y <= 0) y = 59;
						now_tm->tm_sec = y;
						changed = 1;
						break;
					}
				}
			}
			while(REG_KEYIN != 0) { vbl(); }
			vbl();
		}

		goto reset;
	}

	REG_CARET = 0;

	while(REG_KEYIN != 0) { vbl(); }

	while(REG_KEYIN == 0)
	{
		now_t = REG_TIMET;
		now_tm = gmtime(&now_t);

		DrawClock(now_tm);

		TEXT->SetCursorPosition(0, 8);
		printf("Time_T: %llu\nAscTime: \"%s\"\n", now_t, asctime(now_tm));
		printf("GMTime:\n  sec   %d\n  min   %d\n  hour  %d\n  mday  %d\n  mon   %d\n  year  %d\n  wday  %d\n  yday  %d\n", now_tm->tm_sec, now_tm->tm_min, now_tm->tm_hour, now_tm->tm_mday, now_tm->tm_mon, now_tm->tm_year, now_tm->tm_wday, now_tm->tm_yday);

		Spinner();
	}

	while(REG_KEYIN != 0) { vbl(); }
}