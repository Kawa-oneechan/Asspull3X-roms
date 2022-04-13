GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)
GFXAPI := title.png
TILED := background.json

export TARGET :=	tetris
export SFILES :=	crt0.s $(TILED:.json=.s) imfdata.s
export CFILES :=	main.c ../imfplay.c controller.c drawer.c game.c grid.c helpers.c tetramino.c $(GRAPHICS:.png=.c) $(GFXAPI:.png=.c) ../ass-rt.c
include ../ass_romrules
