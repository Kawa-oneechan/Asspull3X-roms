        .text
	.long   0x41535321
	jmp	initialize
	.asciz	"Line Printer test"
	.align	32
	#include "../crt0.s"
