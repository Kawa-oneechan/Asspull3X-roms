#include "../ass.h"
IBios* interface;

int main(void)
{
	REG_SCREENFADE = 31;

	int ret;
	DIR dir;
	FILEINFO info;

	ret = DISK->FindFirst(&dir, &info, "0:", "*.api");

	while(ret == 0 && info.fname[0])
	{
		char picData[info.fsize];
		FILE file;
		DISK->OpenFile(&file, info.fname, FA_READ);
		DISK->ReadFile(&file, (void*)picData, info.fsize);
		DISK->CloseFile(&file);

		if (((TPicFile*)&picData)->bitDepth != 4 && ((TPicFile*)&picData)->bitDepth != 8)
			continue;
		DRAW->DisplayPicture((TPicFile*)&picData);
		//PALETTE[15] = 0x7FFF;
		//DRAW->SetupDrawChar(0);
		//DRAW->DrawString(info.fname, 9, 9, 0);
		//DRAW->DrawString(info.fname, 8, 8, 15);
		DRAW->Fade(true, false);
		while (INP_KEYIN == 0) vbl();
		DRAW->Fade(false, false);
		ret = DISK->FindNext(&dir, &info);
	}
	REG_SCREENMODE = SMODE_TEXT | SMODE_240 | SMODE_BOLD;
	DRAW->ResetPalette();
	TEXT->ClearScreen();
	printf("The end!\n");
	DRAW->Fade(true, false);
	return 0;
}
