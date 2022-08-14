	.text

	.global sprites
	.global spritePals
	.global portraits
	.global uiBackground
	.global fonts

sprites:
	.long 0, farahTiles, iliraTiles, 0,0,0,0,0
	.long 0,0,0,0,0,0,0,0
	.long 0,0,0,0,0,0,0,0
	.long 0,0,0,0,0,0,0,0
spritePals:
	.byte 0,0,1,0,0,0,0,0
	.byte 0,0,0,0,0,0,0,0
	.byte 0,0,0,0,0,0,0,0
	.byte 0,0,0,0,0,0,0,0

portraits:
	.long farah_prtTiles, ilira_prtTiles, kay_prtTiles,0,0,0,0,0
	.long farah_prtPal, ilira_prtPal, kay_prtPal,0,0,0,0,0	

uiBackground:
	.short 0x5294,0x5294,0x5294,0x5294,0x5294,0x5294,0x4E73,0x4E73
	.short 0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73
	.short 0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73
	.short 0x4E73,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52
	.short 0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52
	.short 0x4A52,0x4A52,0x4A52,0x4A52,0x4631,0x4631,0x4631,0x4631
	.short 0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631
	.short 0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4210,0x4210
	.short 0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210
	.short 0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210
	.short 0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210
	.short 0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210
	.short 0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210
	.short 0x4210,0x4210,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631
	.short 0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631
	.short 0x4631,0x4631,0x4631,0x4631,0x4A52,0x4A52,0x4A52,0x4A52
	.short 0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52
	.short 0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4E73
	.short 0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73
	.short 0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73
	.short 0x4E73,0x4E73,0x5294,0x5294,0x5294,0x5294,0x5294,0x5294
	.short 0x5294,0x5294,0x5294,0x5294,0x5294,0x5294,0x4E73,0x4E73
	.short 0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73
	.short 0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73,0x4E73
	.short 0x4E73,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52
	.short 0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52,0x4A52
	.short 0x4A52,0x4A52,0x4A52,0x4A52,0x4631,0x4631,0x4631,0x4631
	.short 0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4631
	.short 0x4631,0x4631,0x4631,0x4631,0x4631,0x4631,0x4210,0x4210
	.short 0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210,0x4210

font1:
	.incbin "999.fon"
font2:
	.incbin "chicago.fon"
font3:
	.incbin "geneva.fon"
font4:
	.incbin "monaco.fon"
fonts:
	.long font1, font2, font3, font4
