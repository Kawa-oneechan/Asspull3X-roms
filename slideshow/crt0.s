	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Slideshow"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"pix0"
	.byte	0	//reserved
	#include "../crt0.s"
