#include "../ass.h"
#include "../lab/std.h"

typedef struct
{
	unsigned char left, top, width, height;
	unsigned short* bits;
} tWindow;

typedef struct
{
	char* title;
	char state;
	char code;
} tMenuItem;

typedef struct
{
	char* title;
	char numItems;
	const tMenuItem* items;
} tMenu;

extern void WaitForKey();
extern tWindow* OpenWindow(int left, int top, int width, int height, int color);
extern void CloseWindow(tWindow* win);
extern void ShowError(const char* message);
extern void DrawPanel(int left, int top, int width, int height, int color);
extern void DrawKeys(const char** keys);
extern void DrawMenu();
extern void OpenMenu();

extern int StartApp(char* filePath);
extern int ShowPic(char* filePath);
extern int ShowText(char* filePath);
extern int ShowFile(char* filePath);

extern char *strrchr(const char *, int);
