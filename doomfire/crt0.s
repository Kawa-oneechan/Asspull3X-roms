	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Doom Fire"
	.align	32
	#include "../crt0_disk.s"
