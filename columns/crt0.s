	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Columns"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"COLw"
	.byte	0	//reserved
	#include "../crt0.s"
