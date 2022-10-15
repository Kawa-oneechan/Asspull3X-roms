//Prototypes for library functions
extern int Write(const char*, ...);
extern int Format(char*, const char*, ...);
extern int VFormat(char*, const char*, va_list);
extern void WriteChar(char);
extern void SetCursorPosition(int, int);
extern void SetTextColor(int, int);
extern void ClearScreen(void);
extern void ResetPalette(void);
extern void DisplayPicture(TImageFile*);
extern void Fade(bool, bool);
extern void SetupDrawChar(int(*)(unsigned char, int, int, int));
extern void DrawString(const char*, int, int, int);
extern void DrawFormat(const char*, int, int, int, ...);
extern int DrawChar(char, int, int, int);
extern void DrawLine(int, int, int, int, int, uint8_t*);
extern void FloodFill(int, int, int, uint8_t*);
extern void WaitForVBlank(void);
extern void WaitForVBlanks(int);
extern void DmaCopy(void*, const void*, size_t, int);
extern void DmaClear(void*, int, size_t, int);
extern void MidiReset(void);
extern void OplReset(void);
extern void RleUnpack(int8_t*, int8_t*, size_t);
extern char* GetLocaleStr(ELocale, int);
extern EFileError OpenFile(TFileHandle*, const char*, char);
extern EFileError CloseFile(TFileHandle*);
extern int ReadFile(TFileHandle*, void*, size_t);
extern int WriteFile(TFileHandle*, void*, size_t);
extern uint32_t SeekFile(TFileHandle*, uint32_t, int);
extern EFileError TruncateFile(TFileHandle*);
extern EFileError FlushFile(TFileHandle*);
extern uint32_t FilePosition(TFileHandle*);
extern bool FileEnd(TFileHandle*);
extern size_t FileSize(TFileHandle*);
extern EFileError OpenDir(TDirHandle*, const char*);
extern EFileError CloseDir(TDirHandle*);
extern EFileError ReadDir(TDirHandle*, TFileInfo*);
extern EFileError FindFirst(TDirHandle*, TFileInfo*, const char*, const char*);
extern EFileError FindNext(TDirHandle*, TFileInfo*);
extern EFileError FileStat(const char*, TFileInfo*);
extern EFileError UnlinkFile(const char*);
extern EFileError RenameFile(const char*, const char*);
extern EFileError FileTouch(const char*, TFileInfo*);
extern EFileError FileAttrib(const char* path, char);
extern EFileError MakeDir(const char*);
extern EFileError ChangeDir(const char*);
extern EFileError GetCurrentDir(char*, size_t);
extern EFileError GetLabel(char, char*, unsigned long*);
extern uint32_t GetFree(char);
extern const char* FileErrStr(EFileError);



extern int DrawChar4_320(unsigned char, int, int, int);
extern int DrawChar4_640(unsigned char, int, int, int);
extern int DrawChar8_320(unsigned char, int, int, int);
extern int DrawChar8_640(unsigned char, int, int, int);
