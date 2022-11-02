GRITS := $(wildcard *.grit)
GRAPHICS := $(GRITS:.grit=.png)

GFX8x8FP := tileset.png
# GFX8x8NP := tileanim.png
GFX8x8 := farah.png
HDMA := hdma1.png
TILED := map.json

export TARGET :=	tiletest
export SFILES :=	crt0.s $(HDMA:.png=.s) $(TILED:.json=.s)
export CFILES :=	main.c smb1as.c $(GRAPHICS:.png=.c)
include ../ass_romrules
