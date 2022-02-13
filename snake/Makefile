GFX8x8 := tiles.png
HDMA := hdma.png
TILED := title.json

export TARGET :=	snake
export SFILES :=	crt0.s $(HDMA:.png=.s) $(TILED:.json=.s)
export CFILES :=	main.c $(GFX8x8:.png=.c)
include ../ass_romrules
