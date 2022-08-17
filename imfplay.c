#include "ass.h"

#define BYTESWAP(x) x = ((x & 0xFF00) >> 8) | ((x & 0xFF) << 8);

static const uint16_t *imfdata;
static const uint16_t *_imfptr;
static uint16_t _imfwait, _imfsize;
static bool imfLoop = true;

uint16_t imfCycles = 16;

void IMF_Service()
{
	if (!_imfptr)
		return;

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

void IMF_Play()
{
	if (!_imfptr)
		return;
	for (int i = 0; i < imfCycles; i++) IMF_Service();
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
