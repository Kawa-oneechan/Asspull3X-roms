	.global tilesTiles
	.global playerTiles
	.global tilesPal
	.global playerPal
	.text
tilesTiles:
	.incbin "tiles.img.bin"
playerTiles:
	.incbin "player.img.bin"
tilesPal:
	.incbin "tiles.pal.bin"
playerPal:
	.incbin "player.pal.bin"
