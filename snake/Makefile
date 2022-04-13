GRAPHICS := tiles.png
HDMA := hdma.png
TILED := title.json

export TARGET :=	snake
export SFILES :=	crt0.s $(HDMA:.png=.s) $(TILED:.json=.s)
export CFILES :=	main.c $(GRAPHICS:.png=.c)
include ../ass_romrules
