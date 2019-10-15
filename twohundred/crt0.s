        .text
	.long   0x41535321
	jmp	initialize
	.asciz	"320x200?"
	.align	32
	#include "../crt0.s"
