#include "../ass.h"
#include "funcs.h"
#include "ffconf.h" //for FF_VOLUMES

extern int vsprintf(char*, const char*, va_list);

const IDiskLibrary diskLibrary =
{
	OpenFile, CloseFile,
	ReadFile, WriteFile,
	SeekFile, TruncateFile, FlushFile,
	FilePosition, FileEnd, FileSize,
	OpenDir, CloseDir, ReadDir,
	FindFirst, FindNext, FileStat,
	UnlinkFile, RenameFile, FileTouch,
	FileAttrib, MakeDir, ChangeDir,
	GetCurrentDir, GetLabel, FileErrStr,
	GetNumDrives, GetFree
};

FATFS FatFs[FF_VOLUMES] = { 0 };

extern int f_open (FILE* fp, const char* path, char mode);
extern int f_close (FILE* fp);
extern int f_read (FILE* fp, void* buff, size_t btr, unsigned int* br);
extern int f_write (FILE* fp, const void* buff, unsigned int btw, unsigned int* bw);
extern int f_lseek (FILE* fp, int ofs);
extern int f_truncate (FILE* fp);
extern int f_sync (FILE* fp);
extern int f_opendir (DIR* dp, const char* path);
extern int f_closedir (DIR* dp);
extern int f_readdir (DIR* dp, FILEINFO* fno);
extern int f_findfirst (DIR* dp, FILEINFO* fno, const char* path, const char* pattern);
extern int f_findnext (DIR* dp, FILEINFO* fno);
extern int f_mkdir (const char* path);
extern int f_unlink (const char* path);
extern int f_rename (const char* path_old, const char* path_new);
extern int f_stat (const char* path, FILEINFO* fno);
extern int f_chmod (const char* path, char attr, char mask);
extern int f_utime (const char* path, const FILEINFO* fno);
extern int f_chdir (const char* path);
//extern int f_chdrive (const char* path);
extern int f_getcwd (char* buff, size_t len);
extern int f_getfree (const char* path, unsigned long* nclst, FATFS** fatfs);
extern int f_getlabel (const char* path, char* label, unsigned long* vsn);
extern int f_setlabel (const char* label);
extern int f_mount (FATFS* fs, const char* path, char opt);
//extern int f_putc (char c, FILE* fp);
//extern int f_puts (const char* str, FILE* cp);
//extern int f_printf (FILE* fp, const char* str, ...);
//extern char* f_gets (char* buff, size_t len, FILE* fp);


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

int OpenFile(TFileHandle* handle, const char* path, char mode)
{
	return f_open(handle, path, mode);
}

int CloseFile(TFileHandle* handle)
{
	return f_close(handle);
}

int ReadFile(TFileHandle* handle, void* target, size_t length)
{
	size_t read;
	if (length == 0)
		length = FileSize(handle);
	int r = f_read(handle, target, length, (unsigned int*)&read);
	if (r)
		return -r;
	else
		return read;
}

int WriteFile(TFileHandle* handle, void* source, size_t length)
{
	size_t wrote;
	int r = f_write(handle, source, length, (unsigned int*)&wrote);
	if (r)
		return -r;
	else
		return wrote;
}

int SeekFile(TFileHandle* handle, unsigned int offset, int origin)
{
	if (origin == SEEK_CUR)
		f_lseek(handle, handle->fptr + offset);
	else if (origin == SEEK_END)
		f_lseek(handle, handle->obj.objsize + offset);
	else //if (origin == SEEK_SET)
		f_lseek(handle, offset);
	return handle->fptr;
}

int TruncateFile(TFileHandle* handle)
{
	return f_truncate(handle);
}
int FlushFile(TFileHandle* handle)
{
	return f_sync(handle);
}

unsigned int FilePosition(TFileHandle* handle)
{
	return handle->fptr;
}

bool FileEnd(TFileHandle* handle)
{
	return (handle->fptr >= handle->obj.objsize);
}

size_t FileSize(TFileHandle* handle)
{
	return handle->obj.objsize;
}

int OpenDir(TDirHandle* handle, const char* path)
{
	return f_opendir(handle, path);
}

int CloseDir(TDirHandle* handle)
{
	return f_closedir(handle);
}

int ReadDir(TDirHandle* handle, TFileInfo* info)
{
	return f_readdir(handle, info);
}

int FindFirst(TDirHandle* handle, TFileInfo* info, const char* path, const char* pattern)
{
	return f_findfirst(handle, info, path, pattern);
}

int FindNext(TDirHandle* handle, TFileInfo* info)
{
	return f_findnext(handle, info);
}

int FileStat(const char* path, TFileInfo* info)
{
	return f_stat(path, info);
}

int UnlinkFile(const char* path)
{
	return f_unlink(path);
}

int RenameFile(const char* from, const char* to)
{
	return f_rename(from, to);
}

#ifdef ALLOW_TOUCH
int FileTouch(const char* path, TFileInfo* dt)
{
	return f_utime(path, dt);
}
#else
int FileTouch(const char* path, TFileInfo* dt)
{
	return 0;
}
#endif

int FileAttrib(const char* path, char attrib)
{
	return f_chmod(path, attrib, 0xFF);
}

int MakeDir(const char* path)
{
	return f_mkdir(path);
}

int ChangeDir(const char* path)
{
	return f_chdir(path);
}

int GetCurrentDir(char* buffer, size_t buflen)
{
	return f_getcwd(buffer, buflen);
}

int GetLabel(char disk, char* buffer, unsigned long* id)
{
	char path[4] = "X:";
	path[0] = disk;
	return f_getlabel(path, buffer, id);
}

int GetFree(char disk)
{
	char path[4] = "X:";
	path[0] = disk;
	FATFS *fs;
	unsigned long freeClusters;
	f_getfree(path, &freeClusters, &fs);
	return freeClusters * (fs->csize * 512);
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

const char* FileErrStr(int error)
{
	if (error > 19) error = 20;
	return FSErrors[error];
}

unsigned char GetNumDrives()
{
	return diskDrives;
}
