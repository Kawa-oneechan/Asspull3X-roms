        .text
	.long   0x41535321
	jmp	initialize
	.asciz	"Snake?"
	.align	32
	#include "../crt0.s"
