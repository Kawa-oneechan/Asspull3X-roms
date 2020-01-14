	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Slideshow"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"FRN0"
	.byte	0	//No SRAM
	#include "../crt0.s"
