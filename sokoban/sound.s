	.include "../ass-macros.i"
	.global jingleSound
	.global slideSound
	.global stepSound
	.global imfData
	.global imfSize
	.text
jingleSound:
	incwav "jingle.wav"
slideSound:
	incwav "slide.wav"
stepSound:
	incwav "step.wav"
imfData:
	.incbin "K5T10.imf"
imfSize:
	.short imfSize - imfData
