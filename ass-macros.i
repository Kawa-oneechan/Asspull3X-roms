.macro incwav fn
	# get the length and swap it
	.incbin "\fn",43,1
	.incbin "\fn",42,1
	.incbin "\fn",41,1
	.incbin "\fn",40,1
	# get the rest of the file
	.incbin "\fn",44	
.endm
