	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Tetris"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"TETw"
	.byte	0	//reserved
	#include "../crt0.s"
