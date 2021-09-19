	.global fontTiles
	.global fontPal
	.global playerTiles
	.global playerPal
	.text
fontTiles:
	.incbin "font.img.bin"
fontPal:
	.incbin "font.pal.bin"
playerTiles:
	.incbin "player.img.bin"
playerPal:
	.incbin "player.pal.bin"
