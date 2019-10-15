        .text
	.long   0x41535321
	jmp	initialize
	.asciz	"MIDI Keyboard"
	.align	32
	#include "../crt0.s"
