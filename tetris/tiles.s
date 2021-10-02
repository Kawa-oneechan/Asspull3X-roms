	.global tilesTiles
	.global farahTiles
	.global logoTiles
	.global tilesPal
	.global farahPal
	.global titlePic
	.global imfData1
	.text
tilesTiles:
	.incbin "tiles.img.bin"
farahTiles:
	.incbin "farah.img.bin"
logoTiles:
	.incbin "logo.img.bin"
tilesPal:
	.incbin "tiles.pal.bin"
farahPal:
	.incbin "farah.pal.bin"
titlePic:
	.incbin "title.api"
imfData1:
	.incbin "techtris.imf"
