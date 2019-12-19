	.global jingleSound
	.global jingleLength
	.global slideSound
	.global slideLength
	.global stepSound
	.global stepLength
	.text
jingleSound:
	.incbin "jingle.raw"
jingleLength:
	.int jingleLength - jingleSound
slideSound:
	.incbin "slide.raw"
slideLength:
	.int slideLength - slideSound
stepSound:
	.incbin "step.raw"
stepLength:
	.int stepLength - stepSound
