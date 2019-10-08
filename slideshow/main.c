#include "../ass.h"
IBios* interface;

void* LoadFile(const char* path, void* buffer, int32_t len)
{
	FILE file;
	FILEINFO nfo;
	int regs = REG_INTRMODE;
	REG_INTRMODE |= 0x80;
	int32_t ret = DISK->FileStat(path, &nfo);

	ret = DISK->OpenFile(&file, path, FA_READ);
	if (ret > 0)
	{
		REG_INTRMODE = regs;
		return (void*)ret;
	}

	void *target = buffer;
	for(;;)
	{
		ret = DISK->ReadFile(&file, target, 1024);
		if (ret < 0)
		{
			REG_INTRMODE = regs;
			return (void*)ret;
		}

		target += ret;
		if (ret < 1024)
			break;
	}

	DISK->CloseFile(&file);
	REG_INTRMODE = regs;
	return buffer;
}

void WaitForKey()
{
	while (REG_KEYIN == 0) vbl();
	while (REG_KEYIN != 0) vbl();
}

int32_t main(void)
{
	interface = (IBios*)(0x01000000);

	int ret;
	DIR dir;
	FILEINFO info;
	REG_INTRMODE |= 0x80;

	TImageFile* image = NULL;
	ret = DISK->FindFirst(&dir, &info, "0:", "*.api");

	while(ret == 0 && info.fname[0])
	{
		image = malloc(info.fsize);
		if (image == NULL)
			continue;

		LoadFile((const char*)info.fname, (void*)image, info.fsize);
		if (image->BitDepth != 4 && image->BitDepth != 8)
			continue;
		DRAW->DisplayPicture(image);
		//PALETTE[15] = 0x7FFF;
		//DRAW->DrawString(info.fname, 9, 9, 0);
		//DRAW->DrawString(info.fname, 8, 8, 15);
		DRAW->FadeFromBlack();
		free(image);
		WaitForKey();
		DRAW->FadeToBlack();
		ret = DISK->FindNext(&dir, &info);
	}
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	TEXT->ClearScreen();
	printf("The end!\n");
	DRAW->FadeFromBlack();
	return 0;
}
