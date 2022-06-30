	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Line Printer test"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"prn0"
	.byte	0	//reserved
	#include "../crt0.s"
