	.include "../ass-macros.i"
	.global jingleSound
	.global slideSound
	.global stepSound
	.global imfData1, imfData2, imfData3, imfData4, imfData5, imfData6
	.text
jingleSound:
	incwav "jingle.wav"
slideSound:
	incwav "slide.wav"
stepSound:
	incwav "step.wav"
imfData1:
	.incbin "blattes.imf"
imfData2:
	.incbin "coolboy.imf"
imfData3:
	.incbin "flute-1.imf"
imfData4:
	.incbin "lot_bon.imf"
imfData5:
	.incbin "secretclan.imf"
imfData6:
	.incbin "sornettes.imf"
