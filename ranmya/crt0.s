	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Here's Ranmya!"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"r120"
	.byte	0	//reserved
	#include "../crt0.s"
