	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"A3X BASIC"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"BASw"
	.byte	0	//reserved
	#include "../crt0.s"
