GFX8x8FP := tileset.png
GFX8x8NP := tileanim.png
GFX16x32 := farah.png
HDMA := hdma1.png
TILED := map.json

export TARGET :=	tiletest
export SFILES :=	crt0.s $(HDMA:.png=.s) $(TILED:.json=.s)
export CFILES :=	main.c $(GFX8x8FP:.png=.c) $(GFX8x8NP:.png=.c) $(GFX16x32:.png=.c)
include ../ass_romrules
