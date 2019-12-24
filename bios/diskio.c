#include "diskio.h"
#include "ff.h"

//TODO: allow multiple drives

//Keeping this separate from ass.h because you REALLY have no business here!
#define DISKRAM			((unsigned char*)0x02000200)
#define REG_DISKSECTOR	*(unsigned short*)(0x02000002)
#define REG_DISKCONTROL	*(volatile unsigned char*)(0x02000004)
#define REG_DISKTRACKS	*(unsigned short*)(0x02000010)
#define REG_DISKHEADS	*(unsigned short*)(0x02000012)
#define REG_DISKSECTORS	*(unsigned short*)(0x02000014)
#define DCTL_PRESENT	1
#define DCTL_ERROR		2
#define DCTL_READNOW	4
#define DCTL_WRITENOW	8
#define REG_DMASOURCE	*(volatile unsigned long*)(0x0D800020)
#define REG_DMATARGET	*(volatile unsigned long*)(0x0D800024)
#define REG_DMALENGTH	*(volatile unsigned long*)(0x0D800028)
#define REG_DMACONTROL	*(volatile unsigned char*)(0x0D80002A)

/*
DSTATUS disk_initialize(BYTE driveNo)
{
	if (REG_DISKCONTROL & DCTL_PRESENT)
		return 0;
	return STA_NOINIT; //STA_NODISK?
}
DSTATUS disk_status(BYTE driveNo) __attribute__ ((weak, alias ("disk_initialize")));
*/
/*
DSTATUS disk_status(BYTE driveNo)
{
	if (REG_DISKCONTROL & DCTL_PRESENT)
		return 0;
	return STA_NOINIT; //STA_NODISK?
}
*/

DRESULT disk_read(BYTE driveNo, BYTE *buff, DWORD sector, UINT count)
{
	while(count--)
	{
		REG_DISKSECTOR = sector++;
		REG_DISKCONTROL = DCTL_READNOW;
		if (REG_DISKCONTROL & DCTL_ERROR)
			return RES_PARERR;
		REG_DMASOURCE = (int)DISKRAM;
		REG_DMATARGET = (int)buff;
		REG_DMALENGTH = 512;
		REG_DMACONTROL = 0x07;
		buff += 512;
	}
	return 0;
}

DRESULT disk_write(BYTE driveNo, const BYTE *buff, DWORD sector, UINT count)
{
	while(count--)
	{
		REG_DISKSECTOR = sector++;
		REG_DMASOURCE = (int)buff;
		REG_DMATARGET = (int)DISKRAM;
		REG_DMALENGTH = 512;
		REG_DMACONTROL = 0x07;
		REG_DISKCONTROL = DCTL_WRITENOW;
		buff += 512;
	}
	return 0;
}

DRESULT disk_ioctl(BYTE driveNo, BYTE ctrl, void *buff)
{
	if (!(REG_DISKCONTROL & DCTL_PRESENT))
		return RES_NOTRDY;
	switch (ctrl)
	{
		case CTRL_SYNC: break;
		case GET_SECTOR_COUNT: *(DWORD*)buff = (REG_DISKTRACKS * REG_DISKHEADS) * REG_DISKSECTORS; break;
		case GET_SECTOR_SIZE: *(WORD*)buff = 512; break;
		case GET_BLOCK_SIZE: *(DWORD*)buff = 1; break;
	}
	return RES_OK;
}
