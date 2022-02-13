GFX8x8 := tiles.png
GFX32x32 := farah.png logo.png
GFXAPI := title.png
TILED := background.json

export TARGET :=	tetris
export SFILES :=	crt0.s $(TILED:.json=.s) imfdata.s
export CFILES :=	main.c imfplay.c controller.c drawer.c game.c grid.c helpers.c tetramino.c $(GFX8x8:.png=.c) $(GFX32x32:.png=.c) $(GFXAPI:.png=.c) ../ass-rt.c
include ../ass_romrules
