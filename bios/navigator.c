#include "nav.h"

extern void ScreenSaverTick();

void PrintBuffer(char* buffer)
{
	unsigned char* lpt = interface->linePrinter;
	if (lpt)
	{
		while (*buffer)
			*lpt = *buffer++;

		*lpt = 0x0A;
		*lpt = 0x0C;
		return;
	}
	ShowError("Could not find a printer.");
}

void PrintFile(char* filePath)
{
	FILEINFO nfo;
	FileStat(filePath, &nfo);
	size_t size = nfo.fsize;
	if (nfo.fattrib & AM_DIRECTORY)
	{
		ShowError("Cannot print a directory.");
		//TODO: list its contents, maybe?
		return;
	}

	char fileText[size];
	FILE file;
	OpenFile(&file, filePath, FA_READ);
	ReadFile(&file, (void*)fileText, nfo.fsize);
	CloseFile(&file);
	fileText[size] = 0;
	PrintBuffer(fileText);
}

char* filenames = { 0 };
int fileCt = 0;

void Populate(const char* path)
{
	EFileError ret;
	DIR dir;
	FILEINFO info;
	fileCt = 0;
	char* curFN = 0;

	if (filenames == 0)
	{
		filenames = (char*)HeapAlloc(MAXFILES * 16);
	}
	curFN = filenames;

	ret = OpenDir(&dir, path);
	if (ret)
	{
		char msg[256];
		Format(msg, "Disk error reading %s:\n  %s", path, (ret == FE_NoDisk) ? "No disk inserted?" : FileErrStr(ret));
		MessageBox(msg, 0);
		//TODO: ask for another drive instead.
		strcpy(curFN, "<ERROR>");
		fileCt++;
		return;
	}

	if (strlen(path) > 3)
	{
		strcpy(curFN, "..");
		curFN += 16;
		fileCt++;
	}

	while (fileCt < MAXFILES)
	{
		ret = ReadDir(&dir, &info);
		if (ret != 0 || info.fname[0] == 0) break;
		if (info.fattrib & AM_DIRECTORY && !(info.fattrib & AM_HIDDEN))
		{
			strcpy(curFN, info.fname);
			curFN += 16;
			fileCt++;
		}
	}

	ret = FindFirst(&dir, &info, path, "*.*");
	while(ret == 0 && info.fname[0] && fileCt < MAXFILES)
	{
		if (!(info.fattrib & AM_HIDDEN))
		{
			strcpy(curFN, info.fname);
			curFN += 16;
			fileCt++;
		}
		ret = FindNext(&dir, &info);
	}
}

char lastInfoDrive = 0;

void InfoPanel(char* workPath, char* filename)
{
	char filePath[MAXPATH];
	FILEINFO info;

	if (lastInfoDrive != workPath[0])
	{
		lastInfoDrive = workPath[0];
		char label[12] = { 0 };
		uint32_t id = 0;
		SetTextColor(SplitColor(CLR_PANEL));
		SetCursorPosition(INFOLEFT + 2, 17);
		GetLabel(workPath[0], label, &id);
		Write("ID: %04X-%04X, %s", id >> 16, id & 0xFFFF, label[0] ? label : "no name");
		SetCursorPosition(INFOLEFT + 2, 18);
		Write("%#d bytes free", GetFree(workPath[0]));
	}


	strcpy(filePath, workPath);
	if (filePath[strlen(filePath) - 1] != '\\') strkitten(filePath, '\\');
	strcat(filePath, filename);
	FileStat(filePath, &info);

	char t[15][28] = { 0 };
	int i = 0;

	if (filename[0] == 0)
	{
		strcpy(t[0], "No file selected.");
	}
	else if (filename[0] == '.' && filename[1] == '.')
		strcpy(t[0], "Go up one directory");
	else
	{
		Format(t[i++], "@%s", filename);
		if (info.fattrib & AM_DIRECTORY)
			strcpy(t[i++], "Directory");
		else
		{
			Format(t[i++], "%#d bytes", info.fsize);

			tm ftime = {
				(info.ftime & 31) << 1, //seconds
				(info.ftime >> 5) & 63, //minutes
				(info.ftime >> 11) & 31, //hours
				info.fdate & 0x1F, //day
				((info.fdate >> 5) & 15) - 1, //month
				(info.fdate >> 9) + 80, //year
				0, 0, 0
			};
			FormatTime(t[i++], 28, "%F %T", &ftime);
			i++;

			char* ext = strrchr((const char*)filename, '.') + 1;
			if (!strncmp(ext, "APP", 3)) strcpy(t[i++], "A\x11\xD7 Application");
			else if (!strncmp(ext, "API", 3))
			{
				strcpy(t[i++], "A\x11\xD7 Image");
				i++;
				TPicFile picHeader;
				FILE imgFile;
				OpenFile(&imgFile, filePath, FA_READ);
				ReadFile(&imgFile, (void*)&picHeader, sizeof(TPicFile));
				CloseFile(&imgFile);
				Format(t[i++], "Bit depth: %d", picHeader.bitDepth);
				if (picHeader.flags & 1) strcpy(t[i++], "Compressed");
				if (picHeader.flags & 2) strcpy(t[i++], "Has HDMA data");
				Format(t[i++], "Size: %d\xD7%d", picHeader.width, picHeader.height);
			}
			else if (!strncmp(ext, "TXT", 3)) strcpy(t[i++], "Text file");
			else if (!strncmp(ext, "FNT", 3) && info.fsize == 12288) strcpy(t[i++], "Font - open to use");
			else if (!strncmp(ext, "LOC", 3) && info.fsize == sizeof(TLocale)) strcpy(t[i++], "Locale - open to use");
			else strcpy(t[i++], "File");
		}
	}

	for (i = 0; i < 15; i++)
	{
		SetCursorPosition(INFOLEFT + 2, 1 + i);
		SetTextColor(CLR_PANEL >> 4, t[i][0] == '@' ? 14 : (CLR_PANEL & 0x0F));
		Write("%-28s", t[i] + (t[i][0] == '@' ? 1 : 0));
	}

	if (filename[0] && filename[0] != '.')
	{
		char attribFlags[] = { AM_ARCHIVE, AM_READONLY, AM_SYSTEM, AM_HIDDEN };
		char attribChars[] = { 'A', 'R', 'S', 'H' };
		SetCursorPosition(INFOLEFT + 26, 1);
		for (i = 0; i < 4; i++)
		{
			SetTextColor(CLR_PANEL >> 4, (info.fattrib & attribFlags[i]) ? CLR_PANEL & 0x0F : 0x00);
			WriteChar(attribChars[i]);
		}
	}
}

void SelectFile(const char* startingPath)
{
	//Note: these are PLACEHOLDERS
	static const char* keys[] = {
		"Drive ",
		"Attrib",
		"View  ",
		"Edit  ",
		"Copy  ",
		"RenMov",
		"Mkdir ",
		"Delete",
		"Print ",
		"Config",
	};

	int index = 0, lastIndex = 0, redraw = 1, scroll = 0, currentDrive;
	FILEINFO info;
	char currDirs[4][MAXPATH], workPath[MAXPATH];
	char filePath[MAXPATH];
	char* curFN;
	bool justAppeared = true;

	for (int i = 0; i < 4; i++)
	{
		Format(currDirs[i], "%c:\\", i + 'A');
	}

	currentDrive = startingPath[0] - 'A';
	strcpy(filePath, currDirs[currentDrive]);
	strcpy(workPath, startingPath);
	Populate(workPath);

	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	ResetPalette();
	//SetTextColor(0, 7);
	//ClearScreen();

//	int dlgResult = DialogTest();
//	Write("dlgResult: %d\n", dlgResult);
//	WaitForKey();

	for(;;)
	{
		vbl();
		if (redraw)
		{
			interface->vBlank = ScreenSaverTick;
			if (redraw < 2)
			{
				DrawPanel(0, 0, FILEWIDTH, 29, CLR_PANEL);
				DrawPanel(INFOLEFT, 0, INFOWIDTH, 29, CLR_PANEL);
				DrawPanelSeparator(INFOLEFT, 16, INFOWIDTH, CLR_PANEL);
				DrawPanelSeparator(INFOLEFT, 20, INFOWIDTH, CLR_PANEL);
				DrawPanelSeparator(INFOLEFT, 23, INFOWIDTH, CLR_PANEL);
				SetTextColor(SplitColor(CLR_PANEL));
				SetCursorPosition(INFOLEFT + 13, 0);
				Write(" File ");
				SetCursorPosition(INFOLEFT + 13, 16);
				Write(" Disk ");
				SetCursorPosition(INFOLEFT + 13, 20);
				Write(" Cart ");
				lastInfoDrive = 0;
			}

			SetTextColor(SplitColor(CLR_PANEL));
//			for (int i = 0; i < WIDTH - 1; i++)
//				TEXTMAP[80 + i + 1] = 0x9000 | CLR_PANEL; //top edge
			SetTextColor(CLR_PANEL & 0x0F, CLR_PANEL >> 4);
			SetCursorPosition((FILEWIDTH / 2) - (strlen(workPath) / 2), 0);
			Write(" %s ", workPath);

			//DrawPanelSeparator(0, FILESSHOWN + 2, WIDTH, CLR_PANEL);
			curFN = &filenames[scroll * 16];
			if (fileCt == 0)
			{
				SetCursorPosition(1, 1);
				SetTextColor(SplitColor(CLR_PANELITEM));
				Write("Disk is empty.");
			}
			else for (int i = 0; i < FILESSHOWN; i++)
			{
				if (i >= fileCt)
				{
					SetCursorPosition(1, i + 1);
					SetTextColor(SplitColor(CLR_PANELITEM));
					Write("%38c", ' ');
					continue;
				}

				strcpy(filePath, workPath);
				if (filePath[strlen(filePath) - 1] != '\\') strkitten(filePath, '\\');
				strcat(filePath, curFN);
				FileStat(filePath, &info);
				SetCursorPosition(1, i + 1);
				SetTextColor(SplitColor(CLR_PANELITEM));
				//if (index == i + scroll)
				//	SetTextColor(SplitColor(CLR_PANELSEL));
				Write(" %-12s  ", curFN);
				if (curFN[0] == '.' && curFN[1] == '.')
				{
					Write("      <UP>            ");
					curFN += 16;
					if (lastIndex != -1 && lastIndex == i + scroll)
						Highlight(1, i + 1, WIDTH - 1, CLR_PANELITEM);
					if (index == i + scroll)
						Highlight(1, i + 1, WIDTH - 1, CLR_PANELSEL);
					continue;
				}
				if (info.fattrib & AM_DIRECTORY)
					Write("     <DIR> ");
				else
					Write("%10d ", info.fsize);
				int fdy = 1980 + (info.fdate >> 9);
				int fdm = (info.fdate >> 5) & 15;
				int fdd = info.fdate & 0x1F;
				Write(" %02d-%02d-%02d ", fdy, fdm, fdd);
				curFN += 16;
				if (lastIndex != -1 && lastIndex == i + scroll)
					Highlight(1, i + 1, WIDTH - 1, CLR_PANELITEM);
				if (index == i + scroll)
					Highlight(1, i + 1, WIDTH - 1, CLR_PANELSEL);
			}

			InfoPanel(workPath, &filenames[index * 16]);

			SetTextColor(0, 7);
			redraw = 0;

			if (redraw < 2)
			{
				DrawKeys(keys);
			}

			if (justAppeared)
			{
				justAppeared = false;
				Fade(true, false);
			}
		}
		else
		{
			if (lastIndex != -1)
				Highlight(1, 1 + lastIndex - scroll, WIDTH - 1, CLR_PANELITEM);
			if (lastIndex != index)
			{
				Highlight(1, 1 + index - scroll, WIDTH - 1, CLR_PANELSEL);
				InfoPanel(workPath, &filenames[index * 16]);
			}
		}
		curFN = &filenames[index * 16];

		//TODO: command prompt here?
		//SetCursorPosition(0, 28);
		//Write("%s>%s", workPath, "lol");

		while(1)
		{
			vbl();

			strcpy(filePath, workPath);
			if (filePath[strlen(filePath)-1] != '\\') strkitten(filePath, '\\');
			strcat(filePath, curFN);

			key = INP_KEYIN;
			if ((key & 0xFF) > 0)
			{
				if (key & 0x200)
				{
				}
				else if (key == KEYSCAN_UP)
				{
					if (fileCt == 0) continue;
					if (index == 0)
					{
						lastIndex = 1;
						break;
					}
					lastIndex = index;
					if (index > 0)
					{
						index--;
						if (index < scroll)
						{
							scroll -= FILESSHOWN;
							if (scroll < 0) scroll = 0;
							redraw = 2;
							break;
						}
					}
					else
					{
						index = fileCt - 1;
						if (fileCt > FILESSHOWN)
						{
							scroll = fileCt - FILESSHOWN;
							redraw = 2;
						}
						else
							scroll = 0;
						break;
					}
				}
				else if (key == KEYSCAN_DOWN)
				{
					if (fileCt == 0) continue;
					if (index == fileCt - 1)
					{
						lastIndex = -1;
						break;
					}
					lastIndex = index;
					if (index < fileCt - 1)
					{
						index++;
						if (index - scroll >= FILESSHOWN)
						{
							scroll += 1;
							if (scroll + FILESSHOWN > fileCt) scroll = fileCt - FILESSHOWN;
							redraw = 2;
							break;
						}
					}
					else
					{
						index = 0;
						scroll = 0;
						if (fileCt >= FILESSHOWN) redraw = 1;
						break;
					}
				}
				else if (key == KEYSCAN_PGUP)
				{
					if (fileCt == 0) continue;
					lastIndex = index;
					index-= FILESSHOWN;
					scroll -= FILESSHOWN;
					if (index < 0) index = 0;
					if (scroll < 0) scroll = 0;
					redraw = 2;
					break;
				}
				else if (key == KEYSCAN_PGDN)
				{
					if (fileCt == 0) continue;
					lastIndex = index;
					index += FILESSHOWN - 1;
					if (index >= fileCt)
						index = fileCt - 1;
					if (index + scroll > FILESSHOWN)
					{
						scroll = index;
						if (scroll + 1 >= fileCt)
						{
							scroll -= FILESSHOWN - 1;
						}
					}
					redraw = 2;
					break;
				}
				else if (key == KEYSCAN_ENTER)
				{
					if (fileCt == 0) continue;
					if (curFN[0] == '.' && curFN[1] == '.')
					{
						char *lastSlash = strrchr(workPath, '\\');
						char justLeft[16] = { 0 };
						strcpy(justLeft, lastSlash + 1);
						int32_t lsPos = lastSlash - workPath;
						workPath[lsPos] = 0;
						if (workPath[0] == 0) strcpy(workPath, "\\");
						strcpy(currDirs[currentDrive], workPath);
						Populate(workPath);
						redraw = 2;
						index = 0;
						scroll = 0;
						for (int r = 0; r < fileCt; r++)
						{
							if (!strncmp(&filenames[r * 16], justLeft, 16))
							{
								//FileStat(&filenames[r * 16], &info);
								//if (info.fattrib & AM_DIRECTORY)
								{
									index = r;
									if (index >= FILESSHOWN)
									{
										scroll = index - FILESSHOWN + 1;
									}
									break;
								}
							}
						}
					}
					else
					{
						FileStat(filePath, &info);
						if (info.fattrib & AM_DIRECTORY)
						{
							strcpy(workPath, filePath);
							strcpy(currDirs[currentDrive], workPath);
							Populate(workPath);
							redraw = 2;
							index = 0;
							scroll = 0;
						}
						else
						{
							if (filenames != 0)
							{
								HeapFree(filenames);
								filenames = 0;
							}
							int32_t ret = ShowFile(filePath, true);
							Populate(workPath);
							redraw = (ret == 2);
							if (redraw)
							{
								SetTextColor(0, 7);
								ClearScreen();
							}
						}
					}
				}
				else if (key == KEYSCAN_F1)
				{
					int oD = currentDrive;
					int nD = SwitchDrive(oD);
					if (oD != nD)
					{
						currentDrive = nD;
						strcpy(filePath, currDirs[currentDrive]);
						strcpy(workPath, filePath);
						Populate(workPath);
						redraw = 2;
						DrawPanel(0, 1, WIDTH + 1, FILESSHOWN + 5, CLR_PANEL);
						index = 0;
					}
					else
						lastIndex = -1;
				}
				else if (key == KEYSCAN_F2)
				{
					ChangeAttributes(filePath);
					InfoPanel(workPath, filePath);
					redraw = 0;
				}
				else if (key == KEYSCAN_F3)
				{
					redraw = ShowFile(filePath, false) > 1;
				}
				else if (key == KEYSCAN_F4)
				{
					ShowError("Editor is in development.");
				}
				else if (key == KEYSCAN_F5)
				{
					ShowError("File copying not implemented yet.");
				}
				else if (key == KEYSCAN_F6)
				{
					ShowError("Renaming and moving not implemented yet.");
				}
				else if (key == KEYSCAN_F7)
				{
					char inp[16] = "TEST";
					if (InputBox("Enter directory name:", inp, 12) != NULL && inp[0] != 0)
					{
						EFileError ret = MakeDir(inp);
						if (ret)
						{
							char message[256] = { 0 };
							VFormat(message, "Could not create directory \"%s\".", inp);
							MessageBox(message, 0);
						}
						else
						{
							Populate(workPath);
							redraw = 2;
						}
					}
				}
				else if (key == KEYSCAN_F8)
				{
					ShowError("File deleting not implemented yet.");
				}
				else if (key == KEYSCAN_F9)
				{
					PrintFile(filePath);
				}
				else if (key == KEYSCAN_F10)
				{
					ShowError("Config screen not implemented yet.");
				}
//				else
//					printf("%#X", key);
				break;
			}
		}
	}
}

void Navigator(void)
{
	REG_SCREENFADE = 31;
	REG_CARET = 0;

	char startingPath[8] = "_:\\";
	for (int i = 0; i < interface->io.numDrives; i++)
	{
		volatile uint8_t* firstDisk = (uint8_t*)0x02000000 + (interface->io.diskToDev[i] * 0x8000);
		if (firstDisk[4] & 1)
		{
			startingPath[0] = 'A' + i;
			break;
		}
	}

	while(1)
		SelectFile(startingPath);
}
