	.global fontTiles
	.global fontPal
	.global starfieldTiles
	.global starfieldPal
	.text
fontTiles:
	.incbin "font.img.bin"
fontPal:
	.incbin "font.pal.bin"
starfieldTiles:
	.incbin "starfield.img.bin"
starfieldPal:
	.incbin "starfield.pal.bin"

	.global vicViperTiles
	.global lordBritishTiles
	.global vicViperPal
	.global lordBritishPal
vicViperTiles:
	.incbin "players/vicviper.img.bin"
lordBritishTiles:
	.incbin "players/lordbritish.img.bin"
vicViperPal:
	.incbin "players/vicviper.pal.bin"
lordBritishPal:
	.incbin "players/lordbritish.pal.bin"
