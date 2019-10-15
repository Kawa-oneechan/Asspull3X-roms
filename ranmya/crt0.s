        .text
	.long   0x41535321
	jmp	initialize
	.asciz	"Here's Ranmya!"
	.align	32
	#include "../crt0.s"
