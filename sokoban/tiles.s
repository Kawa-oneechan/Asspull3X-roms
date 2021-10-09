	.global tilesTiles
	.global playerTiles
	.global backTiles
	.global diskettePic
	.global tilesPal
	.global playerPal
	.global diskettePal
	.global backPals
	.global titlePic
	.text
tilesTiles:
	.incbin "tiles.img.bin"
	.incbin "font.img.bin"
playerTiles:
	.incbin "player.img.bin"
diskettePic:
	.incbin "diskette.img.bin"
backTiles:
	.incbin "backs.img.bin"
backPals:
	.hword 0x0000,0x14A5,0x4E73,0x7FFF,0x1021,0x2021,0x3021,0x04A1
	.hword 0x0501,0x0982,0x0423,0x0427,0x042B,0x0C63,0x14A5,0x1CE7
	.hword 0x04A5,0x04E7,0x056B,0x0421,0x0C63,0x14A5,0x0000,0x0000
	.hword 0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000
tilesPal:
	.incbin "tiles.pal.bin"
playerPal:
	.incbin "player.pal.bin"
diskettePal:
	.incbin "diskette.pal.bin"
titlePic:
	.incbin "title.api"

