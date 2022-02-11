	.global tilesetPal
	.global tilesetTiles
	.global questionTiles
	.global grassTiles
	.global farahPal
	.global farahTiles
	.text
tilesetPal:
	.incbin "tileset.pal.bin"
tilesetTiles:
	.incbin "tileset.img.bin"
questionTiles:
	.incbin "question.img.bin"
grassTiles:
	.incbin "grass.img.bin"
farahPal:
	.incbin "farah.pal.bin"
farahTiles:
	.incbin "farah.img.bin"
