	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Malloc test"
	.align	32
	.long	0	//let ASSFIX set this.
	.short	1	//Kawa made this
	.byte	0	//Worldwide
	.byte	0	//No SRAM
	#include "../crt0.s"
