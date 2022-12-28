#include "ass.h"

#define BYTESWAP(x) x = ((x & 0xFF00) >> 8) | ((x & 0xFF) << 8);

static const uint16_t *imfdata;
static const uint16_t *_imfptr;
static uint16_t _imfwait, _imfsize;
static bool imfLoop = true;

void(*nextVBlank)(void);
uint16_t imfCycles = 16;

typedef struct
{
	uint16_t length;
	uint16_t notlengthlol;
	uint16_t priority;
	uint8_t settings[16];
	uint8_t octave;
	uint8_t pitches[];
} audiot;
static audiot *_audiot;
static uint16_t _audiotptr, _audiotlen, _audiotpriority;
static uint8_t _audiotblock;
static bool _audiotnote;

static void IMF_Service()
{
	while ((_imfsize) && (!_imfwait))
	{
		REG_OPLOUT = *_imfptr++;
		_imfwait = *_imfptr++;
		_imfwait = (_imfwait + 127) / 128;
		_imfsize -= 4;
	}
	_imfwait--;
	if (!_imfsize)
	{
		if (imfLoop)
		{
			_imfptr = imfdata;
			_imfsize = (*_imfptr++) & 0xFCFF;
			BYTESWAP(_imfsize);
		} else
		{
			_imfptr = NULL;
		}
		_imfwait = 0;
	}
}

static void IMF_Play()
{
	if (_audiotlen)
	{
		for (int steps = 0; steps < 2; steps++)
		{
			uint8_t pit = _audiot->pitches[_audiotptr];
			if (pit == 0x00)
			{
				REG_OPLOUT = 0xB000 | _audiotblock;
				_audiotnote = false;
			}
			else
			{
				REG_OPLOUT = 0xA000 | pit;
				if (!_audiotnote)
				{
					REG_OPLOUT = 0xB000 | (_audiotblock | 0x20);
					_audiotnote = true;
				}
			}
			_audiotptr++;
			_audiotlen--;
			if (_audiotlen <= 0)
			{
				_audiotlen = 0;
				REG_OPLOUT = 0xB000 | _audiotblock;
				_audiotpriority = 0;
				break;
			}
		}
	}

	if (!_imfptr)
	{
		if (nextVBlank)
			nextVBlank();
		return;
	}

	for (int i = 0; i < imfCycles; i++) IMF_Service();

	if (nextVBlank)
		nextVBlank();
}

void IMF_Install(void(*next)(void))
{
	if (next == NULL)
		next = interface->vBlank;
	nextVBlank = next;
	interface->vBlank = IMF_Play;
}

void IMF_Remove()
{
	interface->vBlank = nextVBlank;
	nextVBlank = NULL;
}

int IMF_LoadSong(const uint16_t *sauce, bool loop)
{
	imfdata = sauce;
	if (imfdata)
	{
		_imfptr = imfdata;
		_imfwait = 0;
		_imfsize = (*_imfptr++) & 0xFCFF;
		BYTESWAP(_imfsize);
	}
	else
	{
		_imfptr = 0;
	}
	imfLoop = loop;
	return 0;
}

void IMF_AudioT(const void *sauce)
{
	audiot* newsnd = (audiot*)sauce;
	uint16_t pri = newsnd->priority;
	uint32_t len = newsnd->length;
	BYTESWAP(len);
	BYTESWAP(pri);

	if (pri < _audiotpriority)
		return;

	_audiot = newsnd;
	_audiotpriority = pri;

	const uint8_t regs[] =
	{
		0x20, 0x23, 0x40, 0x43, 0x60, 0x63, 0x80, 0x83, 0xE0, 0xE3, 0xC0
	};
	for (int i = 0; i < 11; i++)
		REG_OPLOUT = (regs[i] << 8) | _audiot->settings[i];
	_audiotptr = 0;
	_audiotlen = len;
	_audiotblock = (_audiot->octave & 7) << 2;
	_audiotnote = false;
}

