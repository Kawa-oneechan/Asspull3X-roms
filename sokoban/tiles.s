	.global tilesTiles
	.global playerTiles
	.global diskettePic
	.global tilesPal
	.global playerPal
	.global diskettePal
	.global titlePic
	.text
tilesTiles:
	.incbin "tiles.img.bin"
playerTiles:
	.incbin "player.img.bin"
diskettePic:
	.incbin "diskette.img.bin"
tilesPal:
	.incbin "tiles.pal.bin"
playerPal:
	.incbin "player.pal.bin"
diskettePal:
	.incbin "diskette.pal.bin"
titlePic:
	.incbin "../sokoban.api"

