GRAPHICS := tiles.png bg.png
TILED := title.json bgmap.json

export TARGET :=	snake
export SFILES :=	crt0.s $(TILED:.json=.s)
export CFILES :=	main.c $(GRAPHICS:.png=.c)
include ../ass_romrules
