        .text
	.long   0x41535321
	jmp	initialize
	.asciz	"Boot from Disk"
	.align	32
	#include "../crt0_disk.s"
