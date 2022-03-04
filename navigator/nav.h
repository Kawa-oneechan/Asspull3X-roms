#include "../ass.h"
#include "../lab/std.h"

//#define NORTON

#ifndef NORTON
#define CLR_PANEL 0x87
#define CLR_PANELITEM 0x8F
#define CLR_PANELSEL 0x9F
#define CLR_MENUBAR 0x1F
#define CLR_MENUBARKEY 0x1E
#define CLR_MENU 0x1F
#define CLR_MENUITEM 0x1F
#define CLR_MENUITEMKEY 0x1E
#define CLR_MENUDIS 0x17
#define CLR_MENUSEL 0x9F
#define CLR_KEYNUM 0x8E
#define CLR_KEYTEXT 0x8F
#define CLR_DIALOG 0x7F
#define CLR_VIEWBACK 0x07
#define CLR_VIEWSTAT 0x1B
#else
#define CLR_PANEL 0x1B
#define CLR_PANELITEM 0x1B
#define CLR_PANELSEL 0x31
#define CLR_MENUBAR 0x3F
#define CLR_MENUBARKEY 0x3E
#define CLR_MENU 0x30
#define CLR_MENUITEM 0x3F
#define CLR_MENUITEMKEY 0x3E
#define CLR_MENUDIS 0x38
#define CLR_MENUSEL 0x0F
#define CLR_KEYNUM 0x07
#define CLR_KEYTEXT 0x30
#define CLR_DIALOG 0x7F
#define CLR_VIEWBACK 0x1B
#define CLR_VIEWSTAT 0x31
#endif

#define SplitColor(X) X >> 4, X & 0x0F

typedef struct
{
	unsigned char left, top, width, height;
	unsigned short* bits;
} tWindow;

typedef struct
{
	char* title;
	char scan;
	char state;
	char code;
} tMenuItem;

typedef struct
{
	char* title;
	char scan;
	char numItems;
	const tMenuItem* items;
} tMenu;
extern const tMenu menuBar[];
#define NUMMENUS 4

extern void WaitForKey();
extern tWindow* OpenWindow(int left, int top, int width, int height, int color);
extern void CloseWindow(tWindow* win);
extern void ShowError(const char* message);
extern void DrawPanel(int left, int top, int width, int height, int color);
extern void Highlight(char left, char top, char width, unsigned char color);
extern void DrawKeys(const char** keys);
extern void DrawMenu();
extern void OpenMenu(int num);

extern int StartApp(char* filePath);
extern int ShowPic(char* filePath);
extern int ShowText(char* filePath);
extern int ShowFile(char* filePath);

extern char *strrchr(const char *, int);
