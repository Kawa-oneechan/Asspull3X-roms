	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Gradients in images"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"2000"
	.byte	0	//reserved
	#include "../crt0.s"
