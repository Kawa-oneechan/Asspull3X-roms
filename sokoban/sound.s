	.include "../ass-macros.i"
	.global jingleSound
	.global slideSound
	.global stepSound
	.text
jingleSound:
	incwav "jingle.wav"
slideSound:
	incwav "slide.wav"
stepSound:
	incwav "step.wav"
