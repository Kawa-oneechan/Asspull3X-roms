#include "../ass.h"
IBios* interface;

void* LoadFile(const char* path, void* buffer, int32_t len)
{
	FILE file;
	FILEINFO nfo;
	int32_t ret = DISK->FileStat(path, &nfo);
	//printf("%i, %i -> %i\n", ret, nfo.fsize, len);
	//void* mem = malloc(nfo.fsize);
	//printf("%i\n", mem);
	//return 0;
	//if (mem == 0) return 0;
	ret = DISK->OpenFile(&file, path, FA_READ);
	//void* target = mem;
	if (ret > 0) return (void*)ret;
	/*for(;;)
	{
		ret = f_read(&file, target, 1024, &bytesRead);
		if (ret > 0) return (void*)ret;
		target += bytesRead;
		if (bytesRead < 1024)
			break;
	}*/
	if (nfo.fsize < len) len = nfo.fsize;
	//printf("LoadFile: gonna try reading %d bytes to %x.\n", len, buffer);
	ret = DISK->ReadFile(&file, buffer, len);
	if (ret < 0) return (void*)ret;
	DISK->CloseFile(&file);
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
	TImageFile* image = NULL;
	ret = DISK->FindFirst(&dir, &info, "0:", "*.api");
	while(ret == 0 && info.fname[0])
	{
		//dpf("%s\n", info.fname);
		image = malloc(info.fsize);
		if (image == NULL)
		{
			continue;
		}
		LoadFile((const char*)info.fname, (void*)image, info.fsize);
		if (image->BitDepth != 4 && image->BitDepth != 8)
		{
			free(image);
			continue;
		}
		//MISC->DmaClear((void*)0x0E000000, 0, 640 * 480, DMA_SHORT);
		DRAW->DisplayPicture(image);
		//PALETTE[15] = 0x7FFF;
		//DRAW->DrawString(info.fname, 9, 9, 0);
		//DRAW->DrawString(info.fname, 8, 8, 15);
		//DRAW->FadeFromBlack();
		free(image);

		//MISC->WaitForVBlanks(60);
		WaitForKey();
		//DRAW->FadeToBlack();

		ret = DISK->FindNext(&dir, &info);
	}
	MISC->SetTextMode(SMODE_240 | SMODE_BOLD);
	DRAW->ResetPalette();
	TEXT->ClearScreen();
	REG_SCREENFADE = 0;
	printf("The end!\n");
	return 0;
}
