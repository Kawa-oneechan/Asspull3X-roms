	.global tilesTiles
	.global farahTiles
	.global logoTiles
	.global tilesPal
	.global farahPal
	.global titlePic
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
	.incbin "../tetris.api"
