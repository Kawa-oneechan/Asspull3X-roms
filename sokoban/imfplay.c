#include "../ass.h"

#define BYTESWAP(x) x = ((x & 0xFF00) >> 8) | ((x & 0xFF) << 8);

static const unsigned short *imfdata;
static unsigned short imfsize;
static const unsigned short *_imfptr;
static unsigned short _imfwait, _imfsize;
static int _isChunk, imfLoop=1;
static unsigned long _imfticks=0;

static void IMF_Service()
{
	unsigned short value;

	if (!_imfptr)
		return;

	while ((_imfsize) && (!_imfwait))
	{
		value = *_imfptr++;
		_imfwait = *_imfptr++;
		_imfwait = (_imfwait + 128) / 128;
		_imfsize -= 4;
		REG_OPLOUT = value;
	}
	_imfwait--;
	_imfticks++;
	if (!_imfsize)
	{
		if (imfLoop)
		{
			_imfptr = imfdata;
			_imfsize = imfsize;
			if(_isChunk)
			{
				_imfsize = (*_imfptr++) & 0xFCFF;
				BYTESWAP(_imfsize);
			}
			_imfticks = 0;
		} else
			_imfptr = NULL;
		_imfwait = 0;
	}
}

void IMF_Play()
{
	if (!_imfptr)
		return;
	for (int i = 0; i < 16; i++) IMF_Service();
}

int IMF_LoadSong(const unsigned short *sauce, unsigned short size)
{
	imfdata = sauce;
	if (imfdata)
	{
		imfsize = size;
		imfsize -= imfsize & 3L;
		_imfptr = imfdata;
		_imfsize = imfsize;
		_imfwait = 0;
		_isChunk = !(imfdata[0] == 0 && imfdata[1] == 0);
		if (_isChunk)
		{
			_imfsize = (*_imfptr++) & 0xFCFF;
			BYTESWAP(_imfsize);
		}
	}
	return 0;
}
