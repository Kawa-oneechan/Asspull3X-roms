	.include "../ass-macros.i"
	.global backPals
	.global jingleSound
	.global slideSound
	.global stepSound
	.global imfData1, imfData2, imfData3, imfData4, imfData5, imfData6
	.text
backPals:
	.hword 0x0000,0x6DAE,0x7739,0x7FFF,0x1021,0x2021,0x3021,0x04A1
	.hword 0x0501,0x0982,0x0423,0x0427,0x042B,0x0C63,0x14A5,0x1CE7
	.hword 0x04A5,0x04E7,0x056B,0x0421,0x0C63,0x14A5,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
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
