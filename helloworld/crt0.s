	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Hello World!"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"Hi!0"
	.byte	0	//reserved
	#include "../crt0.s"
