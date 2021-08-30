	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Disk Experiments"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"FRN0"
	.byte	0	//reserved
	#include "../crt0.s"
