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
		char stackImage[info.fsize];
		FILE file;
		DISK->OpenFile(&file, info.fname, FA_READ);
		DISK->ReadFile(&file, (void*)stackImage, info.fsize);
		DISK->CloseFile(&file);

		TImageFile* image = (TImageFile*)&stackImage;
		if (image->BitDepth != 4 && image->BitDepth != 8)
			continue;
		DRAW->DisplayPicture(image);
		//PALETTE[15] = 0x7FFF;
		//DRAW->DrawString(info.fname, 9, 9, 0);
		//DRAW->DrawString(info.fname, 8, 8, 15);
		DRAW->Fade(true, false);
		while (INP_KEYIN == 0) vbl();
		DRAW->Fade(false, false);
		ret = DISK->FindNext(&dir, &info);
	}
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	TEXT->ClearScreen();
	printf("The end!\n");
	DRAW->Fade(true, false);
	return 0;
}
