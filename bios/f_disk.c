#include "../ass.h"
#include "funcs.h"
#include "ffconf.h" //for FF_VOLUMES

extern int32_t vsprintf(char*, const char*, va_list);

const IDiskLibrary diskLibrary =
{
	OpenFile, CloseFile,
	ReadFile, WriteFile,
	SeekFile, TruncateFile, FlushFile,
	FilePosition, FileEnd, FileSize,
	OpenDir, CloseDir, ReadDir,
	FindFirst, FindNext, FileStat,
	UnlinkFile, RenameFile, FileTouch,
	MakeDir, ChangeDir, GetCurrentDir,
	GetLabel, FileErrStr, GetNumDrives,
	GetFree
};

FATFS FatFs[FF_VOLUMES] = { 0 };

extern int32_t f_open (FILE* fp, const char* path, char mode);
extern int32_t f_close (FILE* fp);
extern int32_t f_read (FILE* fp, void* buff, uint32_t btr, uint32_t* br);
extern int32_t f_write (FILE* fp, const void* buff, uint32_t btw, uint32_t* bw);
extern int32_t f_lseek (FILE* fp, int32_t ofs);
extern int32_t f_truncate (FILE* fp);
extern int32_t f_sync (FILE* fp);
extern int32_t f_opendir (DIR* dp, const char* path);
extern int32_t f_closedir (DIR* dp);
extern int32_t f_readdir (DIR* dp, FILEINFO* fno);
extern int32_t f_findfirst (DIR* dp, FILEINFO* fno, const char* path, const char* pattern);
extern int32_t f_findnext (DIR* dp, FILEINFO* fno);
extern int32_t f_mkdir (const char* path);
extern int32_t f_unlink (const char* path);
extern int32_t f_rename (const char* path_old, const char* path_new);
extern int32_t f_stat (const char* path, FILEINFO* fno);
//extern int32_t f_chmod (const char* path, char attr, char mask);
extern int32_t f_utime (const char* path, const FILEINFO* fno);
extern int32_t f_chdir (const char* path);
//extern int32_t f_chdrive (const char* path);
extern int32_t f_getcwd (char* buff, uint32_t len);
extern int32_t f_getfree (const char* path, unsigned long* nclst, FATFS** fatfs);
extern int32_t f_getlabel (const char* path, char* label, unsigned long* vsn);
extern int32_t f_setlabel (const char* label);
extern int32_t f_mount (FATFS* fs, const char* path, char opt);
//extern int32_t f_putc (char c, FILE* fp);
//extern int32_t f_puts (const char* str, FILE* cp);
//extern int32_t f_printf (FILE* fp, const char* str, ...);
//extern char* f_gets (char* buff, int32_t len, FILE* fp);


char diskToDev[FF_VOLUMES] = { 0 };
int diskDrives = 0;
void PrepareDiskToDevMapping()
{
	unsigned char* devices = (unsigned char*)0x02000000;
	diskDrives = 0;
	for (char i = 0; i < 16; i++)
	{
		if (*(short*)devices == 0x0144)
		{
			if (diskDrives < FF_VOLUMES)
				diskToDev[diskDrives] = i;
			diskDrives++;
		}
		devices += 0x8000;
	}
	for (int i = 0; i < diskDrives && i < FF_VOLUMES; i++)
	{
		char path[4] = { i + 'A', ':', 0 };
		f_mount(&FatFs[i], path, 1);
	}
}

int32_t OpenFile(TFileHandle* handle, const char* path, char mode)
{
	return f_open(handle, path, mode);
}

int32_t CloseFile(TFileHandle* handle)
{
	return f_close(handle);
}

int32_t ReadFile(TFileHandle* handle, void* target, uint32_t length)
{
	uint32_t read;
	if (length == 0)
		length = FileSize(handle);
	int32_t r = f_read(handle, target, length, &read);
	if (r)
		return -r;
	else
		return read;
}

int32_t WriteFile(TFileHandle* handle, void* source, uint32_t length)
{
	uint32_t wrote;
	int32_t r = f_write(handle, source, length, &wrote);
	if (r)
		return -r;
	else
		return wrote;
}

int32_t SeekFile(TFileHandle* handle, uint32_t offset, int32_t origin)
{
	if (origin == SEEK_CUR)
		f_lseek(handle, handle->fptr + offset);
	else if (origin == SEEK_END)
		f_lseek(handle, handle->obj.objsize + offset);
	else //if (origin == SEEK_SET)
		f_lseek(handle, offset);
	return handle->fptr;
}

int32_t TruncateFile(TFileHandle* handle)
{
	return f_truncate(handle);
}
int32_t FlushFile(TFileHandle* handle)
{
	return f_sync(handle);
}

uint32_t FilePosition(TFileHandle* handle)
{
	return handle->fptr;
}

int32_t FileEnd(TFileHandle* handle)
{
	return (int32_t)(handle->fptr >= handle->obj.objsize);
}

uint32_t FileSize(TFileHandle* handle)
{
	return handle->obj.objsize;
}

int32_t OpenDir(TDirHandle* handle, const char* path)
{
	return f_opendir(handle, path);
}

int32_t CloseDir(TDirHandle* handle)
{
	return f_closedir(handle);
}

int32_t ReadDir(TDirHandle* handle, TFileInfo* info)
{
	return f_readdir(handle, info);
}

int32_t FindFirst(TDirHandle* handle, TFileInfo* info, const char* path, const char* pattern)
{
	return f_findfirst(handle, info, path, pattern);
}

int32_t FindNext(TDirHandle* handle, TFileInfo* info)
{
	return f_findnext(handle, info);
}

int32_t FileStat(const char* path, TFileInfo* info)
{
	return f_stat(path, info);
}

int32_t UnlinkFile(const char* path)
{
	return f_unlink(path);
}

int32_t RenameFile(const char* from, const char* to)
{
	return f_rename(from, to);
}

#ifdef ALLOW_TOUCH
int32_t FileTouch(const char* path, TFileInfo* dt)
{
	return f_utime(path, dt);
}
#else
int32_t FileTouch(const char* path, TFileInfo* dt)
{
	return 0;
}
#endif

int32_t MakeDir(const char* path)
{
	return f_mkdir(path);
}

int32_t ChangeDir(const char* path)
{
	return f_chdir(path);
}

int32_t GetCurrentDir(char* buffer, int32_t buflen)
{
	return f_getcwd(buffer, buflen);
}

int32_t GetLabel(char disk, char* buffer, unsigned long* id)
{
	char path[4] = "X:";
	path[0] = disk;
	return f_getlabel(path, buffer, id);
}

int32_t GetFree(char disk)
{
	char path[4] = "X:";
	path[0] = disk;
	FATFS *fs;
	unsigned long freeClusters;
	int res = f_getfree(path, &freeClusters, &fs);
	//if (res) return -res;
//	return fs->csize;
	return freeClusters * 512; //fs->ssize;
}

//TODO: Improve these a bit.
static const char* const FSErrors[21] =
{
	"Succeeded.",
	"A hard error occurred in the low-level disk I/O layer.",
	"Assertion failed.",
	"The physical drive cannot work.",
	"Could not find the file.",
	"Could not find the path.",
	"The path name format is invalid.",
	"Access denied or directory is full.",
	"Access denied.",
	"The file or directory object is invalid.",
	"The drive is write-protected.",
	"The logical drive number is invalid.",
	"The volume has no work area.",
	"There is no valid FAT volume.",
	"The f_mkfs() aborted due to any problem.",
	//We don't even *use* these. They're impossible to *get*.
	"", //"Could not get a grant to access the volume within defined period.",
	"", //"The operation is rejected according to the file sharing policy.",
	"", //"LFN working buffer could not be allocated.",
	"Too many open files.",
	"Parameter is invalid.",
	"<Invalid error number>"
};

const char* FileErrStr(int32_t error)
{
	if (error > 19) error = 20;
	return FSErrors[error];
}

int GetNumDrives()
{
	return diskDrives;
}
