	.include "../ass-macros.i"
	.global keyboardImage
	.global soundLoop, soundOne, imfData
	.text
keyboardImage:
	.incbin "kbdimg.bin"
soundLoop:
	incwav "heyevery.wav"
soundOne:
	incwav "jingle.wav"
imfData:
	.incbin "lot_bon.imf"
