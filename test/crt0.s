	.text
	.long   0x41535321
	jmp	initialize
	.asciz	"Test Suite"
	.align	32
	.long	0	//let ASSFIX set this.
	.ascii	"tes0"
	.byte	1	//512 KiB of SRAM
	#include "../crt0.s"
