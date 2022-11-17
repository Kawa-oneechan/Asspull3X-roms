#include "nav.h"

int StartApp(char* filePath)
{
	void(*entry)(void) = (void*)0x01002020;
	FILEINFO nfo;
	FileStat(filePath, &nfo);
	FILE file;
	OpenFile(&file, filePath, FA_READ);
	ReadFile(&file, (void*)0x01002000, nfo.fsize);
	CloseFile(&file);
	ClearScreen();
	entry();
	WaitForKey();
	return 2;
}

int ShowPic(char* filePath)
{
	FILEINFO nfo;
	FileStat(filePath, &nfo);
	size_t size = nfo.fsize;

	SetCursorPosition(0, 12);

	char picData[size];
	FILE file;
	OpenFile(&file, filePath, FA_READ);
	ReadFile(&file, (void*)picData, nfo.fsize);
	CloseFile(&file);
	if (((TPicFile*)&picData)->bitDepth != 4 && ((TPicFile*)&picData)->bitDepth != 8)
	{
		ShowError("Weird bitdepth, not happening.");
		return 2;
	}
	REG_SCREENFADE = 31;
	DisplayPicture((TPicFile*)&picData);
	REG_SCREENFADE = 0;
	WaitForKey();
	return 2;
}

const char* textViewerKeys[] = {
	"Help  ", "Unwrap", "      ", "Hex   ", "      ",
	"      ", "      ", "      ", "Print ", "Quit  "
};

#define TABS 8

int ShowText(char* filePath)
{
	int i, j, scroll = 0, lineCt = 0, redraw = 1;
	bool wrap = false, hex = false;

	FILEINFO nfo;
	FileStat(filePath, &nfo);
	size_t size = nfo.fsize;

	uint8_t fileText[size + 8];
	FILE file;
	OpenFile(&file, filePath, FA_READ);
	ReadFile(&file, (void*)fileText, nfo.fsize);
	CloseFile(&file);
	fileText[size] = 0;

	uint8_t wrapText[size + 1024];

	memset(wrapText, 0, size + 1024);
	uint8_t *b = fileText;
	uint8_t *c = wrapText;

	bool forcedNewLine = false;
	if (fileText[size - 1] != 0x0A)
	{
		forcedNewLine = true;
		fileText[size + 0] = 0x0D;
		fileText[size + 1] = 0x0A;
		fileText[size + 2] = 0x00;
	}

	i = 0;
	while (*b != 0)
	{
		if (*b == '\r')
			b++;
		else if (*b == '\n')
		{
			*c++ = *b++;
			lineCt++;
			i = 0;
		}
		else if (*b == '\t')
		{
			i += TABS - (i % TABS);
			b++;
		}
		else
		{
			*c++ = *b++;
			i++;
			if (i == 80)
			{
				*c++ = '\n';
				lineCt++;
				i = 0;
			}
		}
	}

	int32_t cursor = 0;
	uint8_t* d = fileText;

	SetTextColor(SplitColor(CLR_VIEWBACK));
	ClearScreen();


	for (j = 0; j < 80; j++)
		TEXTMAP[j] = CLR_VIEWSTAT;

	while(1)
	{

		if (redraw)
		{
			SetTextColor(SplitColor(CLR_VIEWSTAT));
			SetCursorPosition(0, 0);
			//Write(" %s \t%d/%d $%x, $%x", filePath, scroll, lineCt, b, b - fileText);

			int percent = (int)(scroll * 100) / lineCt;
			Write("%-72s %5d%%", filePath, percent);

			if (!hex)
			{
				c = d + cursor;
				int row = 1;
				int col = 0;
				while (row < 29 && *c != 0)
				{
					if (*c == '\r')
					{
						c++;
						continue;
					}
					if (*c == '\t')
					{
						int nc = col + TABS - (col % TABS);
						while (col < nc)
						{
							TEXTMAP[(row * 80) + col] = 0x2004;
							col++;
						}
						c++;
						continue;
					}
					if (*c == '\n' && col < 80)
					{
						for (; col < 80; col++)
							TEXTMAP[(row * 80) + col] = 0x2004;
						row++;
						col = 0;
					}
					else if (col == 80)
					{
						if (!wrap)
						{
							TEXTMAP[(row * 80) + 79] = 0x1508;
							while (*c != '\n' && *c != '\r') c++;
							c++;
							//continue;
						}
						row++;
						col = 0;
					}
					else
						TEXTMAP[(row * 80) + (col++)] = (*c << 8) | CLR_VIEWBACK;
					c++;
				}
				while (row < 29)
				{
					for (int l = 0; l < 80; l++)
					{
						TEXTMAP[(row * 80) + l] = 0x2000 | CLR_VIEWBACK;
						row++;
					}
				}
			}
			else
			{
				c = d + ((cursor / 16) * 16);
				int row = 1;
				int col = 0;
				const char hex[] = "0123456789ABCDEF";
				const char cols[] = { 7, 10, 13, 16, 21, 24, 27, 30, 35, 38, 41, 44, 49, 52, 55, 58 };
				const char seps[] = { 19, 33, 47 };
				while (row < 29 && *c != 0)
				{
					if (col % 16 == 0)
					{
						for (int l = 0; l < 80; l++)
							TEXTMAP[(row * 80) + l] = 0x2000 | CLR_VIEWBACK;
						for (int l = 0; l < 3; l++)
							TEXTMAP[(row * 80) + seps[l]] = 0x8900 | CLR_VIEWDIM;

						TEXTMAP[(row * 80) + 4] = (hex[((c - d) >> 0) & 0x0F] << 8) | CLR_VIEWBACK;
						TEXTMAP[(row * 80) + 3] = (hex[((c - d) >> 4) & 0x0F] << 8) | CLR_VIEWBACK;
						TEXTMAP[(row * 80) + 2] = (hex[((c - d) >> 8) & 0x0F] << 8) | CLR_VIEWBACK;
						TEXTMAP[(row * 80) + 1] = (hex[((c - d) >> 12) & 0x0F] << 8) | CLR_VIEWBACK;
						TEXTMAP[(row * 80) + 0] = (hex[((c - d) >> 16) & 0x0F] << 8) | CLR_VIEWBACK;
					}

					TEXTMAP[(row * 80) + cols[col] + 1] = (hex[*c & 0x0F] << 8) | CLR_VIEWBACK;
					TEXTMAP[(row * 80) + cols[col] + 0] = (hex[(*c >> 4) & 0x0F] << 8) | CLR_VIEWBACK;

					TEXTMAP[(row * 80) + 63 + col] = (*c > ' ') ? ((*c << 8) | CLR_VIEWBACK) : (('.' << 8) | CLR_VIEWDIM);

					c++;
					col++;
					if (col == 16)
					{
						col = 0;
						row++;
					}

					if (forcedNewLine && (size_t)(c - d) >= size)
						break;
				}
			}
			DrawKeys(textViewerKeys);
			redraw = 0;
		}

		uint16_t key = INP_KEYIN;
		//vbl();
		if ((key & 0xFF) > 0)
		{
			/*if (key == KEYSCAN_LEFT)
			{
				if (scroll > 0)
				{
					scroll -= 10;
					if (scroll < 0)
						scroll = 0;
					redraw = 1;
				}
			}
			else if (key == KEYSCAN_RIGHT)
			{
				if (scroll + MAXLINESSHOWN < lineCt)
				{
					scroll += 10;
					redraw = 1;
				}
			}
			else*/ if (key == KEYSCAN_UP)
			{
				if (scroll > 0)
				{
					cursor -= 2;
					while (cursor >= 0 && d[cursor] != '\n')
						cursor--;
					cursor++;
					scroll--;
					if (cursor < 0)
					{
						cursor = 0;
						scroll = 0;
					}
					redraw = 1;
				}
			}
			else if (key == KEYSCAN_DOWN)
			{
				if (scroll < lineCt - 28)
				{
					while ((uint32_t)cursor < size && d[cursor] != '\n')
						cursor++;
					cursor++;
					scroll++;
					redraw = 1;
				}
			}
			else if (key == KEYSCAN_F1)
				ShowError("Figure it out lol.");
			else if (key == KEYSCAN_F2)
			{
				wrap = !wrap;
				d = wrap ? wrapText : fileText;
				textViewerKeys[1] = wrap ? "Wrap  " : "Unwrap";
				redraw = 1;
			}
			else if (key == KEYSCAN_F4)
			{
				hex = !hex;
				d = hex ? fileText : (wrap ? wrapText : fileText);
				textViewerKeys[3] = hex ? "ASCII " : "Hex   ";
				redraw = 1;
			}
			else if (key == KEYSCAN_F9)
				PrintBuffer((char*)fileText);
			else if (key == KEYSCAN_F10 || key == KEYSCAN_ESCAPE) //F10 or Escape
				break;
		}
	}
	return 2;
}

int LoadFont(char* filePath)
{
	FILEINFO nfo;
	FileStat(filePath, &nfo);
	if (nfo.fsize != 12288)
	{
		ShowError("Invalid font file, wrong size.");
		return 3;
	}
	FILE file;
	OpenFile(&file, filePath, FA_READ);
	ReadFile(&file, (void*)TEXTFONT, 0x3000);
	CloseFile(&file);
	return 0;
}

int LoadLocale(char* filePath)
{
	FILEINFO nfo;
	FileStat(filePath, &nfo);
	if (nfo.fsize != sizeof(TLocale))
	{
		ShowError("Invalid locale file, wrong size.");
		return 3;
	}
	FILE file;
	OpenFile(&file, filePath, FA_READ);
	ReadFile(&file, (void*)&interface->locale, sizeof(TLocale));
	CloseFile(&file);
	return 0;
}

int ShowFile(char* filePath, bool allowRun)
{
	FILEINFO info;
	FileStat(filePath, &info);
	if (info.fattrib & AM_DIRECTORY || (info.fname[0] == 0))
	{
		ShowError("This is a directory.");
		return 0;
	}

	char* ext = strrchr(filePath, '.') + 1;
	if (!strncmp(ext, "TXT", 3))
		ShowText(filePath);
	else if (!strncmp(ext, "API", 3))
		ShowPic(filePath);
	else if (!strncmp(ext, "FNT", 3) && allowRun)
		LoadFont(filePath);
	else if (!strncmp(ext, "LOC", 3) && allowRun)
		LoadLocale(filePath);
	else if (!strncmp(ext, "APP", 3) && allowRun)
		StartApp(filePath);
	else
	{
//		char msg[64];
//		Format(msg, "Unknown file type \"%s\".", ext);
//		ShowError(msg);
//		return 3;
		ShowText(filePath);
	}
	SetTextColor(0, 7);
	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	ResetPalette();
	return 2;
}
