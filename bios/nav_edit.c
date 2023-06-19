#include "nav.h"

/*** support ***/
#define EOF (-1)
#define MIN_GETLINE_CHUNK 64

int getdelim(char** linePtr, int* n, char delim, FILE* file)
{
	int charsAvailable;
	char* readPos;
	if (*linePtr == NULL)
	{
		*n = MIN_GETLINE_CHUNK;
		*linePtr = malloc(*n);
	}
	charsAvailable = *n;
	readPos = *linePtr;
	while (1)
	{
		//int c = getc(file);
		if (FileEnd(file)) return EOF;
		char c;
		ReadFile(file, &c, 1);

		if (c == EOF)
			return EOF;
		if (c == '\r')
			continue;
		else if (c == -1) break; //End of file.
		if (charsAvailable < 2)
		{
			if (*n > MIN_GETLINE_CHUNK) *n *= 2;
			else *n += MIN_GETLINE_CHUNK;
			charsAvailable = *n + *linePtr - readPos;
			*linePtr = realloc(*linePtr, *n);
			readPos = *n - charsAvailable + *linePtr;
		}
		*readPos++ = c;
		charsAvailable--;
		if (c == delim)
		{
			readPos--;
			break;
		}
	}
	*readPos = '\0';
	return (readPos - *linePtr);
}

#define getline(l,n,f) getdelim(l,n,'\n',f)

/*** defines ***/

#define SCREENROWS 26
#define SCREENCOLS 80
#define KILO_VERSION "0.0.1"
#define KILO_TAB_STOP 4

/*** data ***/

typedef struct
{
	int size;
	int rsize;
	char *chars;
	char *render;
} erow;

typedef struct
{
	int cx, cy;
	int rx;
	int rowoff;
	int coloff;
//	int screenrows; //use SCREENROWS
//	int screencols; //use SCREENCOLS
	int numrows;
	erow *row;
	bool dirty;
	char *filename;
	bool redraw;
	int redrawrow;
//	struct termios orig_termios;
} editorConfig;

editorConfig E;

static int editorRowCxToRx(erow *row, int cx);
static void editorInsertChar(int c, bool shift);
static void editorInsertNewline();
static void editorDelChar();
static void editorSave();

/*** terminal ***/

/*** output ***/

static void editorScroll()
{
	E.rx = 0;
	if (E.cy < E.numrows)
	{
		E.rx = editorRowCxToRx(&E.row[E.cy], E.cx);
	}
	if (E.cy < E.rowoff)
	{
		E.rowoff = E.cy;
		E.redraw = true;
	}
	if (E.cy >= E.rowoff + SCREENROWS)
	{
		E.rowoff = E.cy - SCREENROWS + 1;
		E.redraw = true;
	}
	if (E.cx < E.coloff)
	{
		E.coloff = E.cx;
		E.redraw = true;
	}
	if (E.cx >= E.coloff + SCREENCOLS)
	{
		E.coloff = E.cx - SCREENCOLS + 1;
		E.redraw = true;
	}
	if (E.rx < E.coloff)
	{
		E.coloff = E.rx;
	}
	if (E.rx >= E.coloff + SCREENCOLS)
	{
		E.coloff = E.rx - SCREENCOLS + 1;
	}
}

static void put(char *text, int len)
{
	while (len--)
	{
		WriteChar(*text);
		text++;
	}
}

static void editorDrawRows()
{
	if (!E.redraw)
	{
		REG_CARET = 0x8000;
		SetCursorPosition(E.cx, E.cy);
		return;
	}
	E.redraw = false;
	if (E.redrawrow != -1)
	{
		int filerow = E.redrawrow + E.rowoff;
		SetTextColor(0, 15);
		SetCursorPosition(0, E.cy + 1);
		int len = E.row[filerow].rsize - E.coloff;
		if (len < 0) len = 0;
		if (len > SCREENCOLS) len = SCREENCOLS;
		put(&E.row[filerow].render[E.coloff], len);
		E.redrawrow = -1;
		REG_CARET = 0x8000;
		return;
	}
	ClearScreen();
	SetCursorPosition(0, 1);
	int y;
	for (y = 0; y < SCREENROWS; y++)
	{
		int filerow = y + E.rowoff;
	    if (filerow == E.numrows)
		{
			SetTextColor(0, 4);
			WriteChar('~');
		}
		else
		{
			int len = E.row[filerow].rsize - E.coloff;
			if (len < 0) len = 0;
			if (len > SCREENCOLS) len = SCREENCOLS;
			SetTextColor(0, 7);
			put(&E.row[filerow].render[E.coloff], len);
		}
		if (y < SCREENROWS - 1)
		{
			put("\n", 1);
		}
	}
	REG_CARET = 0x8000;
}

static void editorDrawStatusBar()
{
	char status[80];
	char rstatus[80];
	for (int i = 0; i < 80; i++)
		status[i] = ' ';
	sprintf(status, " %s %s", E.filename ? E.filename : "[No Name]", E.dirty ? "(modified)" : "");
	int rlen = sprintf(rstatus, "%d/%d ", E.cy + 1, E.numrows);
	strcpy(status + 80 - rlen, rstatus);

	SetTextColor(SplitColor(CLR_VIEWSTAT));
	SetCursorPosition(0, 0);
	Write(status);
}

static void editorRefreshScreen()
{
	REG_CARET = 0x0000;
	editorScroll();
	editorDrawRows();
	editorDrawStatusBar();
	SetCursorPosition(E.rx - E.rowoff, E.cy - E.coloff + 1);
	vbl();
}

/*** input ***/

static void editorMoveCursor(uint8_t key)
{
	erow *row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
	switch (key)
	{
		case KEYSCAN_LEFT:
			if (E.cx != 0)
			{
				E.cx--;
			}
			else if (E.cy > 0)
			{
				E.cy--;
				E.cx = E.row[E.cy].size;
			}
			break;
		case KEYSCAN_RIGHT:
			if (row && E.cx < row->size)
			{
				E.cx++;
			}
			else if (row && E.cx == row->size)
			{
				E.cy++;
				E.cx = 0;
			}
			break;
		case KEYSCAN_UP:
			if (E.cy != 0)
				E.cy--;
			break;
		case KEYSCAN_DOWN:
			if (E.cy < E.numrows)
				E.cy++;
			break;
	}

	row = (E.cy >= E.numrows) ? NULL : &E.row[E.cy];
	int rowlen = row ? row->size : 0;
	if (E.cx > rowlen)
		E.cx = rowlen;
}

static void editorProcessKeypress()
{
	uint8_t c;
	while ((c = INP_KEYIN) == 0) vbl();
	if (INP_KEYSHIFT & 4)
	{
		switch (c)
		{
			case KEYSCAN_Q:
				break;
			case KEYSCAN_S:
				editorSave();
				break;
		}
	}
	else
	{
		switch (c)
		{
			case KEYSCAN_LEFT:
			case KEYSCAN_RIGHT:
			case KEYSCAN_UP:
			case KEYSCAN_DOWN:
				editorMoveCursor(c);
				break;

			case KEYSCAN_HOME:
				E.cx = 0;
				break;
			case KEYSCAN_END:
				if (E.cy < E.numrows)
					E.cx = E.row[E.cy].size;
				break;

			case KEYSCAN_PGUP:
			case KEYSCAN_PGDN:
			{
				if (c == KEYSCAN_PGUP)
				{
					E.cy = E.rowoff;
				}
				else if (c == KEYSCAN_PGDN)
				{
					E.cy = E.rowoff + SCREENROWS - 1;
					if (E.cy > E.numrows) E.cy = E.numrows;
				}
				int times = SCREENROWS;
				while (times--)
					editorMoveCursor(c == KEYSCAN_PGUP ? KEYSCAN_UP : KEYSCAN_DOWN);
				break;
			}

			case KEYSCAN_ENTER:
				editorInsertNewline();
				break;

			case KEYSCAN_DELETE:
				editorMoveCursor(KEYSCAN_RIGHT);
				//fallthrough
			case KEYSCAN_BACKSP:
				editorDelChar();
				break;

      		default:
				editorInsertChar(c, (INP_KEYSHIFT & 1) == 1);
				break;
		}
	}
}

/*** row operations ***/

static int editorRowCxToRx(erow *row, int cx)
{
	int rx = 0;
	int j;
	for (j = 0; j < cx; j++)
	{
		if (row->chars[j] == '\t')
			rx += (KILO_TAB_STOP - 1) - (rx % KILO_TAB_STOP);
		rx++;
	}
	return rx;
}

static void editorUpdateRow(erow *row)
{
	int tabs = 0;
	int j;

	for (j = 0; j < row->size; j++)
		if (row->chars[j] == '\t') tabs++;

	free(row->render);
	row->render = malloc(row->size + (tabs * (KILO_TAB_STOP + 1)));

	int idx = 0;
	for (j = 0; j < row->size; j++)
	{
	    if (row->chars[j] == '\t')
	    {
			row->render[idx++] = ' ';
			while (idx % KILO_TAB_STOP != 0) row->render[idx++] = ' ';
		}
		else
		{
			row->render[idx++] = row->chars[j];
		}
	}
	row->render[idx] = '\0';
	row->rsize = idx;

	E.redraw = true;
}

static void editorInsertRow(int at, char *s, size_t len)
{
	if (at < 0 || at > E.numrows) return;
	E.row = realloc(E.row, sizeof(erow) * (E.numrows + 1));
	memmove(&E.row[at + 1], &E.row[at], sizeof(erow) * (E.numrows - at));
	E.row[at].size = len;
	E.row[at].chars = malloc(len + 1);
	memcpy(E.row[at].chars, s, len);
	E.row[at].chars[len] = '\0';

	E.row[at].rsize = 0;
	E.row[at].render = NULL;
	editorUpdateRow(&E.row[at]);

	E.numrows++;
	E.dirty = true;
}

static void editorFreeRow(erow *row)
{
	free(row->render);
	free(row->chars);
}

static void editorDelRow(int at)
{
	if (at < 0 || at >= E.numrows) return;
	editorFreeRow(&E.row[at]);
	memmove(&E.row[at], &E.row[at + 1], sizeof(erow) * (E.numrows - at - 1));
	E.numrows--;
	E.dirty = true;
}

static void editorRowInsertChar(erow *row, int at, int c)
{
	if (at < 0 || at > row->size) at = row->size;
	row->chars = realloc(row->chars, row->size + 2);
	for (int i = row->size; i > at; i--)
		row->chars[i] = row->chars[i - 1];
	row->size++;
	row->chars[at] = c;
	editorUpdateRow(row);
	E.dirty = true;
}

static void editorInsertNewline()
{
	if (E.cx == 0)
	{
		editorInsertRow(E.cy, "", 0);
	}
	else
	{
		erow *row = &E.row[E.cy];
		editorInsertRow(E.cy + 1, &row->chars[E.cx], row->size - E.cx);
		row = &E.row[E.cy];
		row->size = E.cx;
		row->chars[row->size] = '\0';
		editorUpdateRow(row);
	}
	E.cy++;
	E.cx = 0;
}

static void editorRowAppendString(erow *row, char *s, size_t len)
{
	row->chars = realloc(row->chars, row->size + len + 1);
	memcpy(&row->chars[row->size], s, len);
	row->size += len;
	row->chars[row->size] = '\0';
	editorUpdateRow(row);
	E.dirty = true;
}

static void editorRowDelChar(erow *row, int at)
{
	if (at < 0 || at >= row->size) return;
	//memmove(&row->chars[at], &row->chars[at + 1], row->size - at);
	for (int i = at; i < row->size; i++)
		row->chars[i] = row->chars[i + 1];
	row->size--;
	editorUpdateRow(row);
	E.dirty = true;
}

/*** editor operations ***/

static void editorInsertChar(int c, bool shift)
{
	if (E.cy == E.numrows)
	{
		editorInsertRow(E.numrows, "", 0);
	}
	editorRowInsertChar(&E.row[E.cy], E.cx, interface->locale.sctoasc[c + (shift ? 128 : 0)]);
	E.cx++;
	E.redrawrow = E.cy;
}

static void editorDelChar()
{
	if (E.cx == 0 && E.cy == 0) return;
	if (E.cy == E.numrows) return;
	erow *row = &E.row[E.cy];
	if (E.cx > 0)
	{
		editorRowDelChar(row, E.cx - 1);
		E.cx--;
	}
	else
	{
		E.cx = E.row[E.cy - 1].size;
		editorRowAppendString(&E.row[E.cy - 1], row->chars, row->size);
		editorDelRow(E.cy);
		E.cy--;
	}
}

/*** file i/o ***/

static char *editorRowsToString(int *buflen)
{
	int totlen = 0;
	int j;
	for (j = 0; j < E.numrows; j++)
		totlen += E.row[j].size + 1;
	*buflen = totlen;
	char *buf = malloc(totlen);
	char *p = buf;
	for (j = 0; j < E.numrows; j++)
	{
		memcpy(p, E.row[j].chars, E.row[j].size);
		p += E.row[j].size;
		*p = '\n';
		p++;
	}
	return buf;
}

static void editorOpen(char *filename)
{
	if (E.filename) free(E.filename);
	E.filename = strdup(filename);
	FILE fp;
	FILEINFO nfo;
	FileStat(filename, &nfo);
	OpenFile(&fp, filename, FA_READ);

	char *line = NULL;
	int linecap = 0;
	int linelen;
	while ((linelen = getline(&line, &linecap, &fp)) != -1)
	{
		while (linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r'))
			linelen--;
		editorInsertRow(E.numrows, line, linelen);
	}
	free(line);

	CloseFile(&fp);
	E.dirty = false;
}

static void editorSave()
{
	if (E.filename == NULL) return;
	int len;
	char *buf = editorRowsToString(&len);
	FILE fp;
	OpenFile(&fp, E.filename, FA_WRITE);
	WriteFile(&fp, buf, len);
	CloseFile(&fp);
	free(buf);
	E.dirty = false;
}

/*** init ***/

static void initEditor()
{
	E.cx = 0;
	E.cy = 0;
	E.rx = 0;
	E.rowoff = 0;
	E.coloff = 0;
	E.numrows = 0;
	E.row = NULL;
	E.dirty = false;
	E.filename = NULL;
	E.redrawrow = -1;
}

int Kilo(char* file)
{
	initEditor();
	editorOpen(file);

	while (true)
	{
		editorRefreshScreen();
		editorProcessKeypress();
	}
}
