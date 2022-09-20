#include "../ass.h"
IBios* interface;

inline void* LoadFile(const char* path, void* buffer)
{
	FILE file;
	FILEINFO nfo;
	int32_t ret = DISK->FileStat(path, &nfo);

	ret = DISK->OpenFile(&file, path, FA_READ);
	if (ret > 0)
		return (void*)ret;

	void *target = buffer;
	for(;;)
	{
		ret = DISK->ReadFile(&file, target, 1024);
		if (ret < 0)
			return (void*)ret;

		target += ret;
		if (ret < 1024)
			break;
	}

	DISK->CloseFile(&file);
	return buffer;
}

inline void WaitForKey()
{
	while (INP_KEYIN == 0) vbl();
}

int main(void)
{
	int ret;
	DIR dir;
	FILEINFO info;

	ret = DISK->FindFirst(&dir, &info, "0:", "*.api");

	while(ret == 0 && info.fname[0])
	{
		char heapImage[info.fsize];
		LoadFile((const char*)info.fname, (void*)heapImage);
		TImageFile* image = (TImageFile*)&heapImage;
		if (image->BitDepth != 4 && image->BitDepth != 8)
			continue;
		DRAW->DisplayPicture(image);
		//PALETTE[15] = 0x7FFF;
		//DRAW->DrawString(info.fname, 9, 9, 0);
		//DRAW->DrawString(info.fname, 8, 8, 15);
		DRAW->Fade(true, false);
		WaitForKey();
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
