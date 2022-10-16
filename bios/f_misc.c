#include "../ass.h"
#include "funcs.h"

extern int vsprintf(char*, const char*, va_list);

const IMiscLibrary miscLibrary =
{
	WaitForVBlank, WaitForVBlanks,
	DmaCopy, DmaClear,
	MidiReset, OplReset,
	RleUnpack,
	GetLocaleStr
};

void WaitForVBlank()
{
	uint32_t tickCount = REG_TICKCOUNT;
	while (REG_TICKCOUNT == tickCount);
//	while(REG_LINE >= 480);
//	while(REG_LINE < 480);
}

void WaitForVBlanks(int vbls)
{
	while(vbls--) WaitForVBlank();
}

//CONSIDER: Replace these with #define macros in ass.h.
void DmaCopy(void* dst, const void* src, size_t size, int step)
{
	REG_DMASOURCE = (int)src;
	REG_DMATARGET = (int)dst;
	REG_DMALENGTH = size;
	REG_DMACONTROL = 0x07 | (step << 4);
}
void DmaClear(void* dst, int src, size_t size, int step)
{
	REG_DMASOURCE = (int)src;
	REG_DMATARGET = (int)dst;
	REG_DMALENGTH = size;
	REG_DMACONTROL = 0x0D | (step << 4);
}

void MidiReset()
{
	for (int i = 0; i < 16; i++)
	{
		REG_MIDIOUT = (0xB0 | i) | 121 << 8; //Reset controllers
		REG_MIDIOUT = (0xB0 | i) | 123 << 8; //All notes off
	}
}

void OplReset()
{
	static const char regs[] = {
		0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
		0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
		0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0xA0, 0xA1,
		0xA2, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
		0xB0, 0xB1, 0xB2, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6,
		0xB7, 0xB8, 0xBD, 0xC0, 0xC1, 0xC2, 0xC2, 0xC3,
		0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xE0, 0xE1, 0xE2,
		0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA,
		0xEB, 0xEC, 0xED, 0xEE, 0xEF, 0xF0, 0xF1, 0xF2,
		0xF3, 0xF4, 0xF5, 0x08, 0x01,
	};
	for (unsigned int i = 0; i < array_size(regs); i++)
		REG_OPLOUT = regs[i] << 8;
}

void RleUnpack(int8_t* dst, int8_t* src, size_t size)
{
	uint8_t data = 0;
	while (size)
	{
		data = *src++;
		if ((data & 0xC0) == 0xC0)
		{
			int8_t len = data & 0x3F;
			data = *src++;
			size--;
			if (data == 0xC0 && len == 0)
				break;
			for (; len > 0; len--)
				*dst++ = data;
		}
		else
		{
			*dst++ = data;
		}
		size--;
	}
}

char* GetLocaleStr(ELocale category, int item)
{
	char* source;
	switch (category)
	{
		case LC_CODE: return interface->locale.code;
		case LC_DAYS: source = interface->locale.wday_name; break;
		case LC_MONS: source = interface->locale.mon_name; break;
		case LC_DAYF: source = interface->locale.wday_nameF; break;
		case LC_MONF: source = interface->locale.mon_nameF; break;
		case LC_DATES: return interface->locale.shortDateFmt;
		case LC_DATEL: return interface->locale.longDateFmt;
		case LC_TIMES: return interface->locale.shortTimeFmt;
		case LC_TIMEL: return interface->locale.longTimeFmt;
		case LC_CURR: return interface->locale.currency;
		default: return "<?>";
	}
	for (; item; item--, source++) for (; *source; source++);
	return source;
}
