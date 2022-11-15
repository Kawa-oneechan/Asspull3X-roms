#include "nav.h"

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

char* filenames[2] = { 0 };
int fileCt[2] = { 0 };

void Populate(const char* path, int side)
{
	EFileError ret;
	DIR dir;
	FILEINFO info;
	fileCt[side] = 0;
	char* curFN = 0;

	if (filenames[side] == 0)
	{
		filenames[side] = (char*)HeapAlloc(MAXFILES * 16);
	}
	curFN = filenames[side];

tryOpenDir:
	ret = OpenDir(&dir, path);
	if (ret)
	{
		//TODO: MessageBox function in nav_dlgs?
		tWindow* error = OpenWindow(-1, -1, 50, 6, 0x1F);
		SetTextColor(1, 15);
		SetCursorPosition(error->left + 2, error->top + 1);
		Write("Disk error reading %s:", path);
		SetTextColor(1, 9);
		SetCursorPosition(error->left + 4, error->top + 2);
		if (ret == FE_NoDisk)
			Write("No disk inserted?");
		else
			Write(DISK->FileErrStr(ret));
		SetTextColor(1, 14);
		SetCursorPosition(error->left + 2, error->top + 4);
		Write("Abort or Retry? >");
		while (1)
		{
			char k = getchar();
			CloseWindow(error);
			if (k == 'a')
			{
				//Ask for another drive instead?
				strcpy(curFN, "<ERROR>");
				fileCt[side]++;
				return;
			}
			else if (k == 'r')
				goto tryOpenDir;
		}
	}

	if (strlen(path) > 3)
	{
		strcpy(curFN, "..");
		curFN += 16;
		fileCt[side]++;
	}

	while (fileCt[side] < MAXFILES)
	{
		ret = ReadDir(&dir, &info);
		if (ret != 0 || info.fname[0] == 0) break;
		if (info.fattrib & AM_DIRECTORY && !(info.fattrib & AM_HIDDEN))
		{
			strcpy(curFN, info.fname);
			curFN += 16;
			fileCt[side]++;
		}
	}

	ret = FindFirst(&dir, &info, path, "*.*");
	while(ret == 0 && info.fname[0] && fileCt[side] < MAXFILES)
	{
		if (!(info.fattrib & AM_HIDDEN))
		{
			strcpy(curFN, info.fname);
			curFN += 16;
			fileCt[side]++;
		}
		ret = FindNext(&dir, &info);
	}
}

void InfoPanel(int panel, char* workPath, char* filename)
{
	char filePath[MAXPATH];
	FILEINFO info;
	int o = (panel == 0 ? 0 : WIDTH + 1);

	strcpy(filePath, workPath);
	if (filePath[strlen(filePath) - 1] != '\\') strkitten(filePath, '\\');
	strcat(filePath, filename);
	FileStat(filePath, &info);

	char t[20][37] = { 0 };
	int i = 0;

	if (filename[0] == '.' && filename[1] == '.')
		strcpy(t[0], "Go up one directory");
	else
	{
		Format(t[i++], "@%s", filename);
		if (info.fattrib & AM_DIRECTORY)
			strcpy(t[i++], "Directory");
		else
		{
			char* ext = strrchr((const char*)filename, '.') + 1;
			if (!strncmp(ext, "APP", 3)) strcpy(t[i++], "A\x11\xD7 Application");
			else if (!strncmp(ext, "API", 3))
			{
				strcpy(t[i++], "A\x11\xD7 Image");
				i++;
				TImageFile imgHeader;
				FILE imgFile;
				OpenFile(&imgFile, filePath, FA_READ);
				ReadFile(&imgFile, (void*)&imgHeader, sizeof(TImageFile));
				CloseFile(&imgFile);
				Format(t[i++], "Bit depth: %d", imgHeader.bitDepth);
				if (imgHeader.flags & 1) strcpy(t[i++], "Compressed");
				if (imgHeader.flags & 2) strcpy(t[i++], "Has HDMA data");
				Format(t[i++], "Size: %d\xD7%d", imgHeader.width, imgHeader.height);
			}
			else if (!strncmp(ext, "TXT", 3)) strcpy(t[i++], "Text file");
			else if (!strncmp(ext, "FNT", 3) && info.fsize == 12288) strcpy(t[i++], "Font - open to use");
			else if (!strncmp(ext, "LOC", 3) && info.fsize == sizeof(TLocale)) strcpy(t[i++], "Locale - open to use");
			else strcpy(t[i++], "File");
		}
		i++;
		if (info.fattrib & AM_READONLY) strcpy(t[i++], "Read-only");
		if (info.fattrib & AM_HIDDEN) strcpy(t[i++], "Hidden");
		if (info.fattrib & AM_SYSTEM) strcpy(t[i++], "System");
		if (info.fattrib & AM_ARCHIVE) strcpy(t[i++], "Archive");
//		Format(t[i++], "%#02X", info.fattrib);
	}

	for (i = 0; i < 20; i++)
	{
		SetCursorPosition(o + 2, 3 + i);
		SetTextColor(CLR_PANEL >> 4, t[i][0] == '@' ? 14 : (CLR_PANEL & 0x0F));
		Write("%-37s", t[i] + (t[i][0] == '@' ? 1 : 0));
	}
}

void SelectFile(const char* path1, const char* path2)
{
	//Note: these are PLACEHOLDERS
	static const char* keys[] = {
		"Left  ",
		"Right ",
		"View  ",
		"Edit  ",
		"Copy  ",
		"RenMov",
		"Mkdir ",
		"Delete",
		"Print ",
		"PullDn",
	};

	int index[2] = { 0 }, lastIndex[2] = { 0 }, redraw = 1, scroll[2] = { 0 }, currentDrive[2];
	FILEINFO info;
	char currDirs[2][4][MAXPATH], workPath[2][MAXPATH];
	char filePath[2][MAXPATH];
	int views[2] = { 0, 0 };
	char* curFN;
	int cs = 0;
	bool justAppeared = true;

	for (int i = 0; i < 4; i++)
	{
		Format(currDirs[0][i], "%c:\\", i + 'A');
		Format(currDirs[1][i], "%c:\\", i + 'A');
	}

	currentDrive[0] = path1[0] - 'A';
	strcpy(filePath[0], currDirs[0][currentDrive[0]]);
	strcpy(workPath[0], path1);
	Populate(workPath[0], 0);

	if (path2 != 0)
	{
		currentDrive[1] = path2[0] - 'A';
		strcpy(filePath[1], currDirs[1][currentDrive[1]]);
		strcpy(workPath[1], path2);
		Populate(workPath[1], 1);
	}

	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	ResetPalette();
	//SetTextColor(0, 7);
	//ClearScreen();

	for(;;)
	{
		vbl();
		if (redraw)
		{
			if (redraw < 2)
			{
				DrawPanel(0, 1, WIDTH + 1, FILESSHOWN + 5, CLR_PANEL);
				DrawPanel(WIDTH + 1, 1, WIDTH + 1, FILESSHOWN + 5, CLR_PANEL);
			}

			for (int s = 0; s < 2; s++)
			{
				int o = (s == 0 ? 0 : WIDTH + 1);
				if (redraw == 2 && cs != s)
					continue;

				if (views[s] == 0)
				{
					SetTextColor(SplitColor(CLR_PANEL));
					for (int i = 0; i < WIDTH - 1; i++)
						TEXTMAP[80 + o + i + 1] = 0x9000 | CLR_PANEL; //top edge
					SetTextColor(CLR_PANEL & 0x0F, CLR_PANEL >> 4);
					SetCursorPosition(o + (WIDTH / 2) - (strlen(workPath[s]) / 2), 1);
					Write(" %s ", workPath[s]);

					char label[12] = { 0 };
					uint32_t id = 0;
					TEXTMAP[(FILESSHOWN + 2) * 80 + o] = 0x8F00 | CLR_PANEL; //|-
					for (int i = 0; i < WIDTH - 1; i++)
						TEXTMAP[(FILESSHOWN + 2) * 80 + o + i + 1] = 0x9000 | CLR_PANEL; //--
					TEXTMAP[(FILESSHOWN + 2) * 80 + o + WIDTH] = 0x8A00 | CLR_PANEL; //-|
					SetTextColor(SplitColor(CLR_PANEL));
					SetCursorPosition(2 + o, FILESSHOWN + 3);
					GetLabel(workPath[s][0], label, &id);
					Write("Label: %04X-%04X, %s", id >> 16, id & 0xFFFF, label[0] ? label : "no name");
					SetCursorPosition(2 + o, FILESSHOWN + 4);
					Write("Space: %#d bytes free", GetFree(workPath[s][0]));

					curFN = &filenames[s][scroll[s] * 16];
					for (int i = 0; i < FILESSHOWN; i++)
					{
						if (i >= fileCt[s])
						{
							SetCursorPosition(1 + o, i + 2);
							SetTextColor(SplitColor(CLR_PANELITEM));
							printf("%38c", ' ');
							continue;
						}

						strcpy(filePath[s], workPath[s]);
						if (filePath[s][strlen(filePath[s]) - 1] != '\\') strkitten(filePath[s], '\\');
						strcat(filePath[s], curFN);
						FileStat(filePath[s], &info);
						SetCursorPosition(1 + o, i + 2);
						SetTextColor(SplitColor(CLR_PANELITEM));
						//if (cs == s && index[s] == i + scroll[s])
						//	SetTextColor(SplitColor(CLR_PANELSEL));
						Write(" %-12s  ", curFN);
						if (curFN[0] == '.' && curFN[1] == '.')
						{
							Write("      <UP>            ");
							curFN += 16;
							if (cs == s && lastIndex[s] != -1 && lastIndex[s] == i + scroll[s])
								Highlight(o + 1, i + 2, WIDTH - 1, CLR_PANELITEM);
							if (cs == s && index[s] == i + scroll[s])
								Highlight(o + 1, i + 2, WIDTH - 1, CLR_PANELSEL);
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
						if (cs == s && lastIndex[s] != -1 && lastIndex[s] == i + scroll[s])
							Highlight(o + 1, i + 2, WIDTH - 1, CLR_PANELITEM);
						if (cs == s && index[s] == i + scroll[s])
							Highlight(o + 1, i + 2, WIDTH - 1, CLR_PANELSEL);
					}
				}
				else if (views[s] == 1)
				{
					SetTextColor(SplitColor(CLR_PANEL));
					SetCursorPosition(o + (WIDTH / 2) - 2 + 1, 1);
					Write(" Info ");
					InfoPanel(s, workPath[s ^ 1], &filenames[s ^ 1][index[s ^ 1] * 16]);
				}

				SetTextColor(0, 7);
				redraw = 0;
			}

			if (redraw < 2)
			{
				DrawKeys(keys);
				DrawMenu();
			}

			if (justAppeared)
			{
				justAppeared = false;
				Fade(true, false);
			}
		}
		else
		{
			int o = (cs == 0 ? 0 : WIDTH + 1);
			if (lastIndex[cs] != -1)
				Highlight(o + 1, 2 + lastIndex[cs] - scroll[cs], WIDTH - 1, CLR_PANELITEM);
			if (lastIndex[cs] != index[cs])
			{
				Highlight(o + 1, 2 + index[cs] - scroll[cs], WIDTH - 1, CLR_PANELSEL);
				if (views[cs ^ 1] == 1)
					InfoPanel(cs ^ 1, workPath[cs], &filenames[cs][index[cs] * 16]);
			}
			o = (cs == 1 ? 0 : WIDTH + 1);
			Highlight(o + 1, 2 + index[cs ^ 1] - scroll[cs ^ 1], WIDTH - 1, CLR_PANELITEM);
		}
		curFN = &filenames[cs][index[cs] * 16];

		//TODO: command prompt here?
		//SetCursorPosition(0, 28);
		//Write("%s>%s", workPath[cs], "lol");

		while(1)
		{
			vbl();

			strcpy(filePath[cs], workPath[cs]);
			if (filePath[cs][strlen(filePath[cs])-1] != '\\') strkitten(filePath[cs], '\\');
			strcat(filePath[cs], curFN);

			uint16_t key = INP_KEYIN;
			if ((key & 0xFF) > 0)
			{
				if (key & 0x200)
				{
					key &= 0xFF;
					for (int i = 0; i < NUMMENUS; i++)
					{
						if (key == menuBar[i].scan)
						{
							key = OpenMenu(i);
							goto HandleMenu;
							break;
						}
					}
				}
				else if (key == KEYSCAN_TAB && views[cs ^ 1] == 0)
				{
					if (views[cs ^ 1] == 0)
						Highlight(cs ? WIDTH + 1 : 0, index[cs] + 2, WIDTH + 1, CLR_PANELITEM);
					cs ^= 1;
					redraw = 2;
					break;
				}
				else if (key == KEYSCAN_UP)
				{
					if (index[cs] == 0)
					{
						lastIndex[cs] = 1;
						break;
					}
					lastIndex[cs] = index[cs];
					if (index[cs] > 0)
					{
						index[cs]--;
						if (index[cs] < scroll[cs])
						{
							scroll[cs] -= FILESSHOWN;
							if (scroll[cs] < 0) scroll[cs] = 0;
							redraw = 2;
							break;
						}
					}
					else
					{
						index[cs] = fileCt[cs] - 1;
						if (fileCt[cs] > FILESSHOWN)
						{
							scroll[cs] = fileCt[cs] - FILESSHOWN;
							redraw = 2;
						}
						else
							scroll[cs] = 0;
						break;
					}
				}
				else if (key == KEYSCAN_DOWN)
				{
					if (index[cs] == fileCt[cs] - 1)
					{
						lastIndex[cs] = -1;
						break;
					}
					lastIndex[cs] = index[cs];
					if (index[cs] < fileCt[cs] - 1)
					{
						index[cs]++;
						if (index[cs] - scroll[cs] >= FILESSHOWN)
						{
							scroll[cs] += 1;
							if (scroll[cs] + FILESSHOWN > fileCt[cs]) scroll[cs] = fileCt[cs] - FILESSHOWN;
							redraw = 2;
							break;
						}
					}
					else
					{
						index[cs] = 0;
						scroll[cs] = 0;
						if (fileCt[cs] >= FILESSHOWN) redraw = 1;
						break;
					}
				}
				else if (key == KEYSCAN_PGUP)
				{
					lastIndex[cs] = index[cs];
					index[cs] -= FILESSHOWN;
					scroll[cs] -= FILESSHOWN;
					if (index[cs] < 0) index[cs] = 0;
					if (scroll[cs] < 0) scroll[cs] = 0;
					redraw = 2;
					break;
				}
				else if (key == KEYSCAN_PGDN)
				{
					lastIndex[cs] = index[cs];
					index[cs] += FILESSHOWN - 1;
					if (index[cs] >= fileCt[cs])
						index[cs] = fileCt[cs] - 1;
					if (index[cs] + scroll[cs] > FILESSHOWN)
					{
						scroll[cs] = index[cs];
						if (scroll[cs] + 1 >= fileCt[cs])
						{
							scroll[cs] -= FILESSHOWN - 1;
						}
					}
					redraw = 2;
					break;
				}
				else if (key == KEYSCAN_ENTER)
				{
					if (curFN[0] == '.' && curFN[1] == '.')
					{
						char *lastSlash = strrchr(workPath[cs], '\\');
						char justLeft[16] = { 0 };
						strcpy(justLeft, lastSlash + 1);
						int32_t lsPos = lastSlash - workPath[cs];
						workPath[cs][lsPos] = 0;
						if (workPath[cs][0] == 0) strcpy(workPath[cs], "\\");
						strcpy(currDirs[cs][currentDrive[cs]], workPath[cs]);
						Populate(workPath[cs], cs);
						redraw = 2;
						index[cs] = 0;
						scroll[cs] = 0;
						for (int r = 0; r < fileCt[cs]; r++)
						{
							if (!strncmp(&filenames[cs][r * 16], justLeft, 16))
							{
								//FileStat(&filenames[cs][r * 16], &info);
								//if (info.fattrib & AM_DIRECTORY)
								{
									index[cs] = r;
									if (index[cs] >= FILESSHOWN)
									{
										scroll[cs] = index[cs] - FILESSHOWN + 1;
									}
									break;
								}
							}
						}
					}
					else
					{
						FileStat(filePath[cs], &info);
						if (info.fattrib & AM_DIRECTORY)
						{
							strcpy(workPath[cs], filePath[cs]);
							strcpy(currDirs[cs][currentDrive[cs]], workPath[cs]);
							Populate(workPath[cs], cs);
							redraw = 2;
							index[cs] = 0;
							scroll[cs] = 0;
						}
						else
						{
							if (filenames[0] != 0)
							{
								HeapFree(filenames[0]);
								filenames[0] = 0;
							}
							if (filenames[1] != 0)
							{
								HeapFree(filenames[1]);
								filenames[1] = 0;
							}
							int32_t ret = ShowFile(filePath[cs], true);
							Populate(workPath[0], 0);
							Populate(workPath[1], 1);
							redraw = (ret == 2);
							if (redraw)
							{
								SetTextColor(0, 7);
								ClearScreen();
							}
						}
					}
				}
				else if (key == KEYSCAN_F1 || key == KEYSCAN_F2)
ChangeDisk:
				{
					int d = key - 0x3B;
					int oD = currentDrive[d];
					int nD = SwitchDrive(d, oD);
					if (oD != nD)
					{
						currentDrive[d] = nD;
						strcpy(filePath[d], currDirs[d][currentDrive[d]]);
						strcpy(workPath[d], filePath[d]);
						Populate(workPath[d], d);
						redraw = 2;
						DrawPanel(cs ? WIDTH + 1 : 0, 1, WIDTH + 1, FILESSHOWN + 5, CLR_PANEL);
						index[d] = 0;
					}
					else
						lastIndex[d] = -1;
				}
				else if (key == KEYSCAN_F3)
				{ key = 11; goto HandleMenu; }
				else if (key == KEYSCAN_F4)
				{ key = 12; goto HandleMenu; }
				else if (key == KEYSCAN_F5)
				{ key = 13; goto HandleMenu; }
				else if (key == KEYSCAN_F6)
				{ key = 14; goto HandleMenu; }
				else if (key == KEYSCAN_F7)
				{ key = 15; goto HandleMenu; }
				else if (key == KEYSCAN_F8)
				{ key = 16; goto HandleMenu; }
				else if (key == KEYSCAN_F9)
				{ key = 17; goto HandleMenu; }
				else if (key == KEYSCAN_F10)
				{
					key = OpenMenu(0);
HandleMenu:
					if (key > 100)
					{
						int side = 0;
						if (key > 110)
						{
							side = 1;
							key -= 10;
						}
						key -= 100;
						tMenuItem* m =  side ? rightMenu : leftMenu;
						tMenuItem* m2 = side ? leftMenu : rightMenu;
						switch (key)
						{
							case 1: //Files view
							case 2: //Info view
								views[side] = key - 1;
								m[0].state = 0;
								m[1].state = 0;
								m[2].state = 0;
								m[3].state = 0;
								m[key - 1].state = CHECKED;
								if (views[side] != 0)
								{
									cs = side ^ 1;
									m2[1].state = DISABLED;
									m2[2].state = DISABLED;
									m2[3].state = DISABLED;
								}
								else
								{
									m2[1].state = 0;
									m2[2].state = 0;
									m2[3].state = 0;
								}
								break;
							case 6: //Drive
							{
								key = 0x3B + side;
								goto ChangeDisk;
							}
						}
						redraw = 1;
					}
					else
					{
						switch (key)
						{
							case 11: //View
								redraw = ShowFile(filePath[cs], false) > 1;
								break;
							case 12: //Edit
								ShowError("Editor unlikely to be implemented.");
								break;
							case 13: //Copy
								ShowError("File copying not implemented yet.");
								break;
							case 14: //RenMov
								ShowError("Renaming and moving not implemented yet.");
								break;
							case 15: //Mkdir
								ShowError("Directory creating not implemented yet.");
								break;
							case 16: //Delete
								ShowError("File deleting not implemented yet.");
								break;
							case 17: //Print
								PrintFile(filePath[cs]);
								break;
							case 18: //Attrib
								ChangeAttributes(filePath[cs]);
								break;
							case 31: //Swap
							{
								int swap1 = index[0];
								index[0] = index[1];
								index[1] = swap1;
								swap1 = lastIndex[0];
								lastIndex[0] = lastIndex[1];
								lastIndex[1] = swap1;
								swap1 = scroll[0];
								scroll[0] = scroll[1];
								scroll[1] = swap1;
								swap1 = currentDrive[0];
								currentDrive[0] = currentDrive[1];
								currentDrive[1] = swap1;
								swap1 = views[0];
								views[0] = views[1];
								views[1] = swap1;
								char swap2[MAXPATH];
								strcpy(swap2, workPath[0]);
								strcpy(workPath[0], workPath[1]);
								strcpy(workPath[1], swap2);
								strcpy(swap2, filePath[0]);
								strcpy(filePath[0], filePath[1]);
								strcpy(filePath[1], swap2);
								for (int i = 0; i < 4; i++)
								{
									strcpy(swap2, currDirs[0][i]);
									strcpy(currDirs[0][i], currDirs[1][i]);
									strcpy(currDirs[1][i], swap2);
								}
								Populate(workPath[0], 0);
								Populate(workPath[1], 1);
								redraw = 2;
							}
						}
					}
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

	char left[8] = "_:\\";
	char right[8] = "_:\\";
	for (int i = 0; i < interface->io.numDrives; i++)
	{
		volatile uint8_t* firstDisk = (uint8_t*)0x02000000 + (interface->io.diskToDev[i] * 0x8000);
		if (firstDisk[4] & 1)
		{
			if (left[0] == '_')
				left[0] = 'A' + i;
			if (right[0] == '_' || right[0] == left[0])
				right[0] = 'A' + i;
		}
	}

	while(1)
		SelectFile(left, right);
}
