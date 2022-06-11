#include "nav.h"

int StartApp(char* filePath)
{
	void(*entry)(void) = (void*)0x01002020;
	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	FILE file;
	DISK->OpenFile(&file, filePath, FA_READ);
	DISK->ReadFile(&file, (void*)0x01002000, nfo.fsize);
	TEXT->ClearScreen();
	entry();
	WaitForKey();
	return 2;
}

int ShowPic(char* filePath)
{
	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	size_t size = nfo.fsize;

	TEXT->SetCursorPosition(0, 12);

	TImageFile* image = malloc(size);
	if (image == NULL)
	{
		ShowError("Failed to malloc.");
		return 2;
	}
	FILE file;
	DISK->OpenFile(&file, filePath, FA_READ);
	DISK->ReadFile(&file, (void*)image, nfo.fsize);
	DISK->CloseFile(&file);
	if (image->BitDepth != 4 && image->BitDepth != 8)
	{
		ShowError("Weird bitdepth, not happening.");
		return 2;
	}
	DRAW->DisplayPicture(image);
	free(image);
	WaitForKey();
	return 2;
}

int ShowText(char* filePath)
{
	//Note: these are PLACEHOLDERS
	static const char* keys[] = {
		"Help  ",
		"Hidden",
		"      ",
		"      ",
		"      ",
		"      ",
		"      ",
		"      ",
		"Print ",
		"Quit  ",
	};

	int i, j, scroll = 0, lineCt = 0, redraw = 1;
	bool hidden = false;

	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	size_t size = nfo.fsize;

	uint8_t* fileText = malloc(size);
	FILE file;
	DISK->OpenFile(&file, filePath, FA_READ);
	DISK->ReadFile(&file, (void*)fileText, nfo.fsize);
	DISK->CloseFile(&file);
	fileText[size] = 0;

	uint8_t* fullText = malloc(size + 1024);
	memset(fullText, 0, size + 1024);
	uint8_t *b = fileText;
	uint8_t *c = fullText;
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

	free(fileText);

	b = fullText;

	TEXT->SetTextColor(SplitColor(CLR_VIEWBACK));
	TEXT->ClearScreen();

	while(1)
	{
		if (redraw)
		{
			TEXT->SetTextColor(SplitColor(CLR_VIEWSTAT));
			for (j = 0; j < 80; j++)
				TEXTMAP[j] = CLR_VIEWSTAT;
			TEXT->SetCursorPosition(0, 0);
			printf(" %s \t%d/%d $%x, $%x", filePath, scroll, lineCt, b, b - fullText);
			c = b;
			int row = 1;
			int col = 0;
			while (row < 29 && *c != 0)
			{
				if (*c == '\n' && col < 80)
				{
					if (hidden)
						TEXTMAP[(row * 80) + col++] = 0x0F04;
					for (; col < 80; col++)
						TEXTMAP[(row * 80) + col] = 0x2004;
					row++;
					col = 0;
				}
				else if (col == 80)
				{
					row++;
					col = 0;
				}
				else
					TEXTMAP[(row * 80) + (col++)] = (*c << 8) | CLR_VIEWBACK;
				c++;
			}
			DrawKeys(keys);
			redraw = 0;
		}

		uint16_t key = INP_KEYIN;
		//vbl();
		if ((key & 0xFF) > 0)
		{
			/*if (key == 0xCB) //left
			{
				if (scroll > 0)
				{
					scroll -= 10;
					if (scroll < 0)
						scroll = 0;
					redraw = 1;
				}
			}
			else if (key == 0xCD) //right
			{
				if (scroll + MAXLINESSHOWN < lineCt)
				{
					scroll += 10;
					redraw = 1;
				}
			}
			else*/ if (key == 0xC8) //up
			{
				if (scroll > 0)
				{
					b -= 2;
					while (b >= fullText && *b != '\n')
						b--;
					b++;
					scroll--;
					if (b < fullText)
					{
						b = fullText;
						scroll = 0;
					}
					redraw = 1;
				}
			}
			else if (key == 0xD0) //down
			{
				if (scroll < lineCt - 28)
				{
					while (b < fullText + size && *b != '\n')
						b++;
					b++;
					scroll++;
					redraw = 1;
				}
			}
			else if (key == 0x3B) //F1
				ShowError("F1 not implemented yet");
			else if (key == 0x3C) //F2
			{
				hidden = !hidden;
				redraw = 1;
			}
			else if (key == 0x3D) //F3
				ShowError("F3 not implemented yet");
			else if (key == 0x3E) //F4
				ShowError("F4 not implemented yet");
			else if (key == 0x3F) //F5
				ShowError("F5 not implemented yet");
			else if (key == 0x40) //F6
				ShowError("F6 not implemented yet");
			else if (key == 0x41) //F7
				ShowError("F7 not implemented yet");
			else if (key == 0x42) //F8
				ShowError("F8 not implemented yet");
			else if (key == 0x43) //F9
				PrintBuffer((char*)fullText);
			else if (key == 0x44 || key == 0x01) //F10 or Escape
				break;
		}
	}
	free(fullText);
	return 2;
}

int ShowFile(char* filePath, bool allowRun)
{
	char* ext = strrchr(filePath, '.') + 1;
	if (!strcmp(ext, "TXT"))
		ShowText(filePath);
	else if (!strcmp(ext, "API"))
		ShowPic(filePath);
	else if (!strcmp(ext, "APP") && allowRun)
		StartApp(filePath);
	else
	{
		char msg[64];
		sprintf(msg, "Unknown file type \"%s\".", ext);
		ShowError(msg);
		return 3;
	}
	TEXT->SetTextColor(0, 7);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	return 2;
}
