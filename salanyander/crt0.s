	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Salanyander"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"GRAw"
	.byte	0	//reserved
	#include "../crt0.s"
