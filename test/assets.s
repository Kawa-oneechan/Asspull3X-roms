	.include "../ass-macros.i"
	.global keyboardImage
	.global soundLoop, soundOne, imfData
	.text
keyboardImage:
	.incbin "kbdimg.bin"
soundLoop:
	incwav "funkydrummer.wav"
soundOne:
	incwav "wow.wav"
imfData:
	.incbin "lot_bon.imf"
