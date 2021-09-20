	.global fontTiles
	.global fontPal
	.global player1Tiles
	.global player2Tiles
	.global player1Pal
	.global player2Pal
	.global starfieldTiles
	.global starfieldPal
	.text
fontTiles:
	.incbin "font.img.bin"
fontPal:
	.incbin "font.pal.bin"
player1Tiles:
	.incbin "player1.img.bin"
player2Tiles:
	.incbin "player2.img.bin"
player1Pal:
	.incbin "player1.pal.bin"
player2Pal:
	.incbin "player2.pal.bin"
starfieldTiles:
	.incbin "starfield.img.bin"
starfieldPal:
	.incbin "starfield.pal.bin"
