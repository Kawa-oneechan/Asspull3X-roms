	.global tilesTiles
	.global tilesPal
//	.global farahTiles
//	.global farahPal
//	.global titlePic
	.text
tilesTiles:
	.incbin "tiles.img.bin"
tilesPal:
	.incbin "tiles.pal.bin"
//farahTiles:
//	.incbin "farah.img.bin"
//farahPal:
//	.incbin "farah.pal.bin"
//titlePic:
//	.incbin "../tetris.api"
