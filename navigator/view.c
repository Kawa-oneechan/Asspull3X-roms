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
	int i, j, scroll = 0, lineCt = 0, redraw = 1;

	FILEINFO nfo;
	DISK->FileStat(filePath, &nfo);
	size_t size = nfo.fsize;

	intoff();

	unsigned char* fileText = malloc(size);
	FILE file;
	DISK->OpenFile(&file, filePath, FA_READ);
	DISK->ReadFile(&file, (void*)fileText, nfo.fsize);
	DISK->CloseFile(&file);

	unsigned char* fullText = malloc(size + 1024);
	unsigned char *b = fileText;
	unsigned char *c = fullText;
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
	TEXT->ClearScreen();

	while(1)
	{
		intoff();
		if (redraw)
		{
			TEXT->SetTextColor(0, 7);
			TEXT->ClearScreen();
			TEXT->SetTextColor(1, 11);
			for (j = 0; j < 80; j++)
				TEXTMAP[j] = 0x1B;
			printf(" %s \t%d/%d $%x, $%x", filePath, scroll, lineCt, b, b - fullText);
			intoff();
			c = b;
			int row = 1;
			int col = 0;
			while (row < 29 && *c != 0)
			{
				if (*c == '\n' && col < 80)
				{
					TEXTMAP[(row * 80) + col] = 0x0F04;
					row++;
					col = 0;
				}
				else if (col == 80)
				{
					row++;
					col = 0;
				}
				else
					TEXTMAP[(row * 80) + (col++)] = (*c << 8) | 0x07;
				c++;
			}
			redraw = 0;
		}

		unsigned short key = REG_KEYIN;
		//vbl();
		if ((key & 0xFF) > 0)
		{
			while(1) { if (REG_KEYIN == 0) break; }

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
				if (scroll < lineCt - 26)
				{
					while (b < fullText + size && *b != '\n')
						b++;
					b++;
					scroll++;
					redraw = 1;
				}
			}
			else if (key == 0x01) //esc
				break;
			else
				printf("%x", key);
		}
	}
	free(fullText);
	return 2;
}

int ShowFile(char* filePath)
{
	char* ext = strrchr(filePath, '.') + 1;
	if (!strcmp(ext, "TXT"))
		ShowText(filePath);
	else if (!strcmp(ext, "API"))
		ShowPic(filePath);
	else if (!strcmp(ext, "APP"))
		StartApp(filePath);
	else
	{
		char msg[64];
		sprintf(msg, "Unknown file type \"%s\".", ext);
		ShowError(msg);
		return 3;
	}
	intoff();
	TEXT->SetTextColor(0, 7);
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	return 2;
}
